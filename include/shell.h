#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

/* Configuration Constants */
#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_JOBS 100
#define PROMPT_SIZE 256

/* ANSI Color Codes */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"

/* Debug Logging */
#define DEBUG 0
#if DEBUG
  #define log_debug(...) fprintf(stderr, "[DEBUG] " __VA_ARGS__)
#else
  #define log_debug(...)
#endif

/* Global Variables */
extern volatile sig_atomic_t fg_pid;
extern char current_dir[MAX_LINE];

/* Function Prototypes */
void display_prompt(void);
char *read_line(void);
void shell_loop(void);
void init_shell(void);

#endif /* SHELL_H */
