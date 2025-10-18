#include "../include/jobs.h"
#include <sys/wait.h>

/* Job list head */
static job_t *job_list = NULL;
static int next_job_id = 1;

/* Initialize job list */
void init_jobs(void) {
    job_list = NULL;
    next_job_id = 1;
}

/* Add a new job to the list */
int add_job(pid_t pid, pid_t pgid, const char *command, job_status_t status) {
    job_t *new_job = malloc(sizeof(job_t));
    if (!new_job) {
        perror("malloc");
        return -1;
    }
    
    new_job->id = next_job_id++;
    new_job->pid = pid;
    new_job->pgid = pgid;
    new_job->command = strdup(command);
    new_job->status = status;
    new_job->next = job_list;
    
    job_list = new_job;
    
    log_debug("Added job [%d] pid=%d pgid=%d: %s\n", 
              new_job->id, new_job->pid, new_job->pgid, new_job->command);
    
    return new_job->id;
}

/* Remove a job from the list */
void remove_job(int job_id) {
    job_t **current = &job_list;
    
    while (*current) {
        if ((*current)->id == job_id) {
            job_t *to_remove = *current;
            *current = (*current)->next;
            
            log_debug("Removing job [%d]: %s\n", to_remove->id, to_remove->command);
            
            free(to_remove->command);
            free(to_remove);
            return;
        }
        current = &(*current)->next;
    }
}

/* Update job status by PID */
void update_job_status(pid_t pid, job_status_t status) {
    job_t *current = job_list;
    
    while (current) {
        if (current->pid == pid || current->pgid == pid) {
            current->status = status;
            log_debug("Updated job [%d] status to %d\n", current->id, status);
            return;
        }
        current = current->next;
    }
}

/* Get job by ID */
job_t *get_job(int job_id) {
    job_t *current = job_list;
    
    while (current) {
        if (current->id == job_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/* Get job by PID or PGID */
job_t *get_job_by_pid(pid_t pid) {
    job_t *current = job_list;
    
    while (current) {
        if (current->pid == pid || current->pgid == pid) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/* Clean up completed jobs */
void cleanup_completed_jobs(void) {
    pid_t pid;
    int status;
    
    /* Check for terminated child processes */
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        job_t *job = get_job_by_pid(pid);
        
        if (job) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                /* Process exited or was killed */
                printf("[%d]+ Done                    %s\n", job->id, job->command);
                remove_job(job->id);
            } else if (WIFSTOPPED(status)) {
                /* Process was stopped */
                update_job_status(pid, JOB_STOPPED);
            } else if (WIFCONTINUED(status)) {
                /* Process was continued */
                update_job_status(pid, JOB_RUNNING);
            }
        }
    }
}

/* List all jobs */
void list_jobs(void) {
    job_t *current = job_list;
    
    if (!current) {
        return;
    }
    
    while (current) {
        const char *status_str;
        switch (current->status) {
            case JOB_RUNNING:
                status_str = "Running";
                break;
            case JOB_STOPPED:
                status_str = "Stopped";
                break;
            case JOB_DONE:
                status_str = "Done";
                break;
            default:
                status_str = "Unknown";
        }
        
        printf("[%d]  %-20s %s\n", current->id, status_str, current->command);
        current = current->next;
    }
}

/* Built-in: jobs command */
int builtin_jobs(void) {
    cleanup_completed_jobs();
    list_jobs();
    return 0;
}

/* Built-in: fg command */
int builtin_fg(char **argv) {
    extern volatile sig_atomic_t fg_pid;
    
    if (!argv[1]) {
        fprintf(stderr, "fg: usage: fg %%<job_id>\n");
        return 1;
    }
    
    /* Parse job ID */
    char *id_str = argv[1];
    if (id_str[0] == '%') {
        id_str++;
    }
    
    int job_id = atoi(id_str);
    job_t *job = get_job(job_id);
    
    if (!job) {
        fprintf(stderr, "fg: %d: no such job\n", job_id);
        return 1;
    }
    
    printf("%s\n", job->command);
    
    /* Continue the job if stopped */
    if (job->status == JOB_STOPPED) {
        kill(-job->pgid, SIGCONT);
    }
    
    job->status = JOB_RUNNING;
    
    /* Give terminal control to the job */
    tcsetpgrp(STDIN_FILENO, job->pgid);
    fg_pid = job->pgid;
    
    /* Wait for the job */
    int status;
    pid_t result = waitpid(-job->pgid, &status, WUNTRACED);
    
    /* Take back terminal control */
    tcsetpgrp(STDIN_FILENO, getpgrp());
    fg_pid = 0;
    
    if (result > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            /* Job completed */
            remove_job(job_id);
        } else if (WIFSTOPPED(status)) {
            /* Job was stopped again */
            job->status = JOB_STOPPED;
            printf("\n[%d]+ Stopped                 %s\n", job->id, job->command);
        }
    }
    
    return 0;
}

/* Built-in: bg command */
int builtin_bg(char **argv) {
    if (!argv[1]) {
        fprintf(stderr, "bg: usage: bg %%<job_id>\n");
        return 1;
    }
    
    /* Parse job ID */
    char *id_str = argv[1];
    if (id_str[0] == '%') {
        id_str++;
    }
    
    int job_id = atoi(id_str);
    job_t *job = get_job(job_id);
    
    if (!job) {
        fprintf(stderr, "bg: %d: no such job\n", job_id);
        return 1;
    }
    
    if (job->status != JOB_STOPPED) {
        fprintf(stderr, "bg: job %d already in background\n", job_id);
        return 1;
    }
    
    /* Continue the job in background */
    kill(-job->pgid, SIGCONT);
    job->status = JOB_RUNNING;
    
    printf("[%d]+ %s &\n", job->id, job->command);
    
    return 0;
}
