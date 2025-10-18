#include "../include/executor.h"
#include "../include/parser.h"
#include "../include/jobs.h"
#include <sys/wait.h>

/* External global variables */
extern volatile sig_atomic_t fg_pid;

/* Check if command is a built-in */
int is_builtin(command_t *cmd) {
    if (!cmd || !cmd->argv || !cmd->argv[0]) return 0;
    
    const char *builtins[] = {"cd", "exit", "jobs", "fg", "bg", "help", NULL};
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(cmd->argv[0], builtins[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Execute built-in command */
int execute_builtin(command_t *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        /* Change directory */
        const char *dir = cmd->argv[1] ? cmd->argv[1] : getenv("HOME");
        if (chdir(dir) != 0) {
            perror("cd");
            return 1;
        }
        return 0;
    }
    else if (strcmp(cmd->argv[0], "exit") == 0) {
        /* Exit shell */
        printf("Exiting myShell...\n");
        exit(0);
    }
    else if (strcmp(cmd->argv[0], "jobs") == 0) {
        return builtin_jobs();
    }
    else if (strcmp(cmd->argv[0], "fg") == 0) {
        return builtin_fg(cmd->argv);
    }
    else if (strcmp(cmd->argv[0], "bg") == 0) {
        return builtin_bg(cmd->argv);
    }
    else if (strcmp(cmd->argv[0], "help") == 0) {
        printf("myShell - Custom Unix-like Shell\n");
        printf("Built-in commands:\n");
        printf("  cd [dir]       - Change directory\n");
        printf("  exit           - Exit shell\n");
        printf("  jobs           - List background jobs\n");
        printf("  fg %%<id>       - Bring job to foreground\n");
        printf("  bg %%<id>       - Resume job in background\n");
        printf("  help           - Show this help message\n");
        printf("\nFeatures:\n");
        printf("  Pipelines: cmd1 | cmd2 | cmd3\n");
        printf("  Redirection: cmd < input.txt > output.txt\n");
        printf("  Background: cmd &\n");
        return 0;
    }
    
    return 1;
}

/* Build command string for job tracking */
char *build_command_string(command_t *cmd) {
    char *result = malloc(MAX_LINE);
    if (!result) return NULL;
    
    result[0] = '\0';
    command_t *current = cmd;
    
    while (current) {
        for (int i = 0; current->argv[i] != NULL; i++) {
            strcat(result, current->argv[i]);
            if (current->argv[i + 1]) strcat(result, " ");
        }
        
        if (current->input_file) {
            strcat(result, " < ");
            strcat(result, current->input_file);
        }
        if (current->output_file) {
            strcat(result, " > ");
            strcat(result, current->output_file);
        }
        
        if (current->next) {
            strcat(result, " | ");
        }
        
        current = current->next;
    }
    
    if (cmd->background) {
        strcat(result, " &");
    }
    
    return result;
}

/* Setup I/O redirections for a command */
void setup_redirections(command_t *cmd) {
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) {
            perror(cmd->input_file);
            exit(EXIT_FAILURE);
        }
        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
    
    if (cmd->output_file) {
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror(cmd->output_file);
            exit(EXIT_FAILURE);
        }
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
}

/* Execute a single command with given input/output fds */
int execute_single(command_t *cmd, int input_fd, int output_fd) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        /* Child process */
        
        /* Set up process group for job control */
        setpgid(0, 0);
        
        /* Redirect input if needed */
        if (input_fd != STDIN_FILENO) {
            if (dup2(input_fd, STDIN_FILENO) < 0) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(input_fd);
        }
        
        /* Redirect output if needed */
        if (output_fd != STDOUT_FILENO) {
            if (dup2(output_fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(output_fd);
        }
        
        /* Setup file redirections */
        setup_redirections(cmd);
        
        /* Execute command */
        execvp(cmd->argv[0], cmd->argv);
        
        /* If execvp returns, an error occurred */
        fprintf(stderr, "myShell: %s: command not found\n", cmd->argv[0]);
        exit(EXIT_FAILURE);
    }
    
    return pid;
}

/* Execute a pipeline of commands */
int execute_pipeline(command_t *cmd) {
    if (!cmd) return -1;
    
    /* Count commands in pipeline */
    int cmd_count = 0;
    command_t *temp = cmd;
    while (temp) {
        cmd_count++;
        temp = temp->next;
    }
    
    /* Single command (no pipeline) */
    if (cmd_count == 1) {
        pid_t pid = execute_single(cmd, STDIN_FILENO, STDOUT_FILENO);
        if (pid < 0) return -1;
        
        /* Set process group */
        setpgid(pid, pid);
        
        if (cmd->background) {
            /* Background job */
            char *cmd_str = build_command_string(cmd);
            add_job(pid, pid, cmd_str, JOB_RUNNING);
            printf("[%d] %d\n", cmd->background, pid);
            free(cmd_str);
        } else {
            /* Foreground job */
            fg_pid = pid;
            tcsetpgrp(STDIN_FILENO, pid);
            
            int status;
            waitpid(pid, &status, WUNTRACED);
            
            /* Restore shell to foreground */
            tcsetpgrp(STDIN_FILENO, getpgrp());
            fg_pid = 0;
            
            if (WIFSTOPPED(status)) {
                /* Job was stopped (Ctrl+Z) */
                char *cmd_str = build_command_string(cmd);
                int job_id = add_job(pid, pid, cmd_str, JOB_STOPPED);
                printf("\n[%d]+ Stopped                 %s\n", job_id, cmd_str);
                free(cmd_str);
            }
        }
        
        return 0;
    }
    
    /* Pipeline execution */
    int pipe_fds[2 * (cmd_count - 1)];
    
    /* Create all pipes */
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("pipe");
            return -1;
        }
    }
    
    pid_t pgid = 0;
    pid_t pids[cmd_count];
    command_t *current = cmd;
    
    for (int i = 0; i < cmd_count; i++) {
        int input_fd = STDIN_FILENO;
        int output_fd = STDOUT_FILENO;
        
        /* Setup input from previous pipe */
        if (i > 0) {
            input_fd = pipe_fds[(i - 1) * 2];
        }
        
        /* Setup output to next pipe */
        if (i < cmd_count - 1) {
            output_fd = pipe_fds[i * 2 + 1];
        }
        
        pid_t pid = execute_single(current, input_fd, output_fd);
        if (pid < 0) return -1;
        
        pids[i] = pid;
        
        /* Set process group (all processes in pipeline share same pgid) */
        if (i == 0) {
            pgid = pid;
        }
        setpgid(pid, pgid);
        
        current = current->next;
    }
    
    /* Close all pipe file descriptors in parent */
    for (int i = 0; i < 2 * (cmd_count - 1); i++) {
        close(pipe_fds[i]);
    }
    
    if (cmd->background) {
        /* Background pipeline */
        char *cmd_str = build_command_string(cmd);
        add_job(pgid, pgid, cmd_str, JOB_RUNNING);
        printf("[%d] %d\n", cmd->background, pgid);
        free(cmd_str);
    } else {
        /* Foreground pipeline */
        fg_pid = pgid;
        tcsetpgrp(STDIN_FILENO, pgid);
        
        /* Wait for all processes in pipeline */
        for (int i = 0; i < cmd_count; i++) {
            int status;
            waitpid(pids[i], &status, WUNTRACED);
            
            if (WIFSTOPPED(status)) {
                /* Pipeline was stopped */
                char *cmd_str = build_command_string(cmd);
                int job_id = add_job(pgid, pgid, cmd_str, JOB_STOPPED);
                printf("\n[%d]+ Stopped                 %s\n", job_id, cmd_str);
                free(cmd_str);
                break;
            }
        }
        
        /* Restore shell to foreground */
        tcsetpgrp(STDIN_FILENO, getpgrp());
        fg_pid = 0;
    }
    
    return 0;
}

/* Main execute function */
int execute_command(command_t *cmd) {
    if (!cmd || !cmd->argv || !cmd->argv[0]) {
        return 0;
    }
    
    /* Clean up completed jobs */
    cleanup_completed_jobs();
    
    /* Handle built-in commands */
    if (is_builtin(cmd)) {
        return execute_builtin(cmd);
    }
    
    /* Execute external command or pipeline */
    return execute_pipeline(cmd);
}
