#include "../include/shell.h"
#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/jobs.h"
#include "../include/signals.h"

/* Global variables */
volatile sig_atomic_t fg_pid = 0;
char current_dir[MAX_LINE];

/* Display shell prompt with current directory */
void display_prompt(void) {
    char hostname[256];
    char *username = getenv("USER");
    
    if (!username) username = "user";
    
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "unknown");
    }
    
    /* Get current working directory */
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        strcpy(current_dir, "~");
    }
    
    /* Replace home directory with ~ */
    char *home = getenv("HOME");
    if (home && strncmp(current_dir, home, strlen(home)) == 0) {
        char temp[MAX_LINE];
        snprintf(temp, sizeof(temp), "~%s", current_dir + strlen(home));
        strcpy(current_dir, temp);
    }
    
    /* Display colored prompt: [user@host:dir] mysh> */
    printf("%s[%s@%s:%s]%s mysh> ", 
           COLOR_GREEN, username, hostname, current_dir, COLOR_RESET);
    fflush(stdout);
}

/* Read a line of input from user */
char *read_line(void) {
    char *line = malloc(MAX_LINE);
    if (!line) {
        perror("malloc");
        return NULL;
    }
    
    if (fgets(line, MAX_LINE, stdin) == NULL) {
        if (feof(stdin)) {
            /* EOF (Ctrl+D) */
            printf("\n");
            free(line);
            exit(0);
        }
        free(line);
        return NULL;
    }
    
    /* Remove trailing newline */
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
    
    return line;
}

/* Initialize shell environment */
void init_shell(void) {
    /* Set shell as process group leader */
    pid_t shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0) {
        perror("setpgid");
        exit(EXIT_FAILURE);
    }
    
    /* Take control of terminal */
    tcsetpgrp(STDIN_FILENO, shell_pgid);
    
    /* Initialize signal handlers */
    setup_signal_handlers();
    
    /* Initialize job list */
    init_jobs();
    
    log_debug("Shell initialized with pid %d\n", getpid());
}

/* Main shell loop */
void shell_loop(void) {
    while (1) {
        /* Display prompt */
        display_prompt();
        
        /* Read user input */
        char *input = read_line();
        if (!input) continue;
        
        /* Skip empty input */
        if (strlen(input) == 0) {
            free(input);
            continue;
        }
        
        log_debug("Input: %s\n", input);
        
        /* Parse command */
        command_t *cmd = parse_command(input);
        free(input);
        
        if (!cmd) continue;
        
        /* Execute command */
        execute_command(cmd);
        
        /* Free command structure */
        free_command(cmd);
    }
}

/* Main entry point */
int main(void) {
    /* Display welcome message */
    printf("%s", COLOR_CYAN);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║     myShell - Custom Unix-like Shell      ║\n");
    printf("║   With Job Control & Pipelining Support   ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("%s", COLOR_RESET);
    printf("\nType 'help' for available commands.\n");
    printf("Type 'exit' to quit.\n\n");
    
    /* Initialize shell */
    init_shell();
    
    /* Start main loop */
    shell_loop();
    
    return 0;
}
