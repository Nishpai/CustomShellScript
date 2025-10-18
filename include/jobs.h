#ifndef JOBS_H
#define JOBS_H

#include "shell.h"

/* Job Status */
typedef enum {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} job_status_t;

/* Job Structure */
typedef struct job_t {
    int id;                   /* Job ID */
    pid_t pid;                /* Process ID */
    pid_t pgid;               /* Process group ID */
    char *command;            /* Command string */
    job_status_t status;      /* Current status */
    struct job_t *next;       /* Next job in list */
} job_t;

/* Job Management Functions */
void init_jobs(void);
int add_job(pid_t pid, pid_t pgid, const char *command, job_status_t status);
void remove_job(int job_id);
void update_job_status(pid_t pid, job_status_t status);
job_t *get_job(int job_id);
job_t *get_job_by_pid(pid_t pid);
void list_jobs(void);
void cleanup_completed_jobs(void);
int builtin_jobs(void);
int builtin_fg(char **argv);
int builtin_bg(char **argv);

#endif /* JOBS_H */
