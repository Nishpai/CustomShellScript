#include "../include/signals.h"
#include "../include/jobs.h"
#include <sys/wait.h>

/* Global variable to track foreground process */
volatile sig_atomic_t fg_pid = 0;

/* SIGCHLD handler - reap zombie processes and update job status */
void sigchld_handler(int sig) {
    (void)sig;  /* Unused parameter */
    
    int saved_errno = errno;
    pid_t pid;
    int status;
    
    /* Reap all terminated or stopped child processes */
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        log_debug("SIGCHLD: pid=%d, status=%d\n", pid, status);
        
        job_t *job = get_job_by_pid(pid);
        
        if (job) {
            if (WIFEXITED(status)) {
                /* Process exited normally */
                log_debug("Process %d exited with status %d\n", pid, WEXITSTATUS(status));
                update_job_status(pid, JOB_DONE);
            } else if (WIFSIGNALED(status)) {
                /* Process was killed by signal */
                log_debug("Process %d killed by signal %d\n", pid, WTERMSIG(status));
                update_job_status(pid, JOB_DONE);
            } else if (WIFSTOPPED(status)) {
                /* Process was stopped */
                log_debug("Process %d stopped by signal %d\n", pid, WSTOPSIG(status));
                update_job_status(pid, JOB_STOPPED);
            } else if (WIFCONTINUED(status)) {
                /* Process was continued */
                log_debug("Process %d continued\n", pid);
                update_job_status(pid, JOB_RUNNING);
            }
        }
    }
    
    errno = saved_errno;
}

/* SIGINT handler (Ctrl+C) - forward to foreground process group */
void sigint_handler(int sig) {
    (void)sig;  /* Unused parameter */
    
    if (fg_pid > 0) {
        /* Forward signal to foreground process group */
        log_debug("SIGINT: forwarding to process group %d\n", fg_pid);
        kill(-fg_pid, SIGINT);
    } else {
        /* No foreground process, just print newline */
        write(STDOUT_FILENO, "\n", 1);
    }
}

/* SIGTSTP handler (Ctrl+Z) - stop foreground process group */
void sigtstp_handler(int sig) {
    (void)sig;  /* Unused parameter */
    
    if (fg_pid > 0) {
        /* Forward signal to foreground process group */
        log_debug("SIGTSTP: forwarding to process group %d\n", fg_pid);
        kill(-fg_pid, SIGTSTP);
    }
}

/* Setup all signal handlers */
void setup_signal_handlers(void) {
    struct sigaction sa_chld;
    struct sigaction sa_int;
    struct sigaction sa_tstp;
    
    /* SIGCHLD handler - reap zombie processes */
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa_chld, NULL) < 0) {
        perror("sigaction(SIGCHLD)");
        exit(EXIT_FAILURE);
    }
    
    /* SIGINT handler - Ctrl+C */
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa_int, NULL) < 0) {
        perror("sigaction(SIGINT)");
        exit(EXIT_FAILURE);
    }
    
    /* SIGTSTP handler - Ctrl+Z */
    sa_tstp.sa_handler = sigtstp_handler;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    if (sigaction(SIGTSTP, &sa_tstp, NULL) < 0) {
        perror("sigaction(SIGTSTP)");
        exit(EXIT_FAILURE);
    }
    
    /* Ignore SIGTTOU to prevent background job from stopping 
       when trying to write to terminal */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    
    log_debug("Signal handlers installed\n");
}
