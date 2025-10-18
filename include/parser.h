#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

/* Command Structure */
typedef struct command_t {
    char **argv;              /* Command arguments (NULL-terminated) */
    char *input_file;         /* Input redirection file (< file) */
    char *output_file;        /* Output redirection file (> file) */
    int background;           /* 1 if command ends with & */
    struct command_t *next;   /* Next command in pipeline */
} command_t;

/* Parser Functions */
command_t *parse_command(char *input);
void free_command(command_t *cmd);
char **tokenize(char *line, const char *delim);
void trim_whitespace(char *str);

#endif /* PARSER_H */
