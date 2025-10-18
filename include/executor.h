#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "shell.h"
#include "parser.h"

/* Executor Functions */
int execute_command(command_t *cmd);
int execute_pipeline(command_t *cmd);
int execute_single(command_t *cmd, int input_fd, int output_fd);
int is_builtin(command_t *cmd);
int execute_builtin(command_t *cmd);
void setup_redirections(command_t *cmd);
char *build_command_string(command_t *cmd);

#endif /* EXECUTOR_H */
