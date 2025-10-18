#include "../include/parser.h"
#include <ctype.h>

/* Trim leading and trailing whitespace from string */
void trim_whitespace(char *str) {
    if (!str) return;
    
    /* Trim leading whitespace */
    char *start = str;
    while (isspace((unsigned char)*start)) start++;
    
    /* Trim trailing whitespace */
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    
    /* Move trimmed string to beginning */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/* Tokenize a string based on delimiter */
char **tokenize(char *line, const char *delim) {
    char **tokens = malloc(MAX_ARGS * sizeof(char*));
    if (!tokens) {
        perror("malloc");
        return NULL;
    }
    
    int i = 0;
    char *token = strtok(line, delim);
    while (token != NULL && i < MAX_ARGS - 1) {
        tokens[i++] = strdup(token);
        token = strtok(NULL, delim);
    }
    tokens[i] = NULL;
    
    return tokens;
}

/* Create a new command structure */
static command_t *create_command(void) {
    command_t *cmd = malloc(sizeof(command_t));
    if (!cmd) {
        perror("malloc");
        return NULL;
    }
    
    cmd->argv = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->background = 0;
    cmd->next = NULL;
    
    return cmd;
}

/* Parse a single command (handling redirections) */
static command_t *parse_single_command(char *input) {
    command_t *cmd = create_command();
    if (!cmd) return NULL;
    
    char *args[MAX_ARGS];
    int arg_count = 0;
    
    /* Tokenize and handle redirections */
    char *token = strtok(input, " \t\n");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            /* Input redirection */
            token = strtok(NULL, " \t\n");
            if (token) {
                cmd->input_file = strdup(token);
            }
        } else if (strcmp(token, ">") == 0) {
            /* Output redirection */
            token = strtok(NULL, " \t\n");
            if (token) {
                cmd->output_file = strdup(token);
            }
        } else {
            /* Regular argument */
            args[arg_count++] = strdup(token);
        }
        token = strtok(NULL, " \t\n");
    }
    args[arg_count] = NULL;
    
    /* Copy arguments to command structure */
    cmd->argv = malloc((arg_count + 1) * sizeof(char*));
    if (!cmd->argv) {
        perror("malloc");
        free_command(cmd);
        return NULL;
    }
    
    for (int i = 0; i <= arg_count; i++) {
        cmd->argv[i] = args[i];
    }
    
    return cmd;
}

/* Main parser: handles pipes, background, and redirections */
command_t *parse_command(char *input) {
    if (!input || strlen(input) == 0) return NULL;
    
    /* Make a copy of input for manipulation */
    char *input_copy = strdup(input);
    if (!input_copy) {
        perror("strdup");
        return NULL;
    }
    
    trim_whitespace(input_copy);
    if (strlen(input_copy) == 0) {
        free(input_copy);
        return NULL;
    }
    
    /* Check for background execution */
    int background = 0;
    size_t len = strlen(input_copy);
    if (input_copy[len - 1] == '&') {
        background = 1;
        input_copy[len - 1] = '\0';
        trim_whitespace(input_copy);
    }
    
    /* Split by pipes */
    char *pipe_tokens[MAX_ARGS];
    int pipe_count = 0;
    
    char *pipe_ptr = input_copy;
    char *pipe_start = pipe_ptr;
    
    while (*pipe_ptr) {
        if (*pipe_ptr == '|') {
            *pipe_ptr = '\0';
            pipe_tokens[pipe_count++] = pipe_start;
            pipe_start = pipe_ptr + 1;
        }
        pipe_ptr++;
    }
    pipe_tokens[pipe_count++] = pipe_start;
    
    /* Parse each command in the pipeline */
    command_t *first_cmd = NULL;
    command_t *prev_cmd = NULL;
    
    for (int i = 0; i < pipe_count; i++) {
        char *cmd_str = strdup(pipe_tokens[i]);
        trim_whitespace(cmd_str);
        
        command_t *cmd = parse_single_command(cmd_str);
        free(cmd_str);
        
        if (!cmd) {
            free_command(first_cmd);
            free(input_copy);
            return NULL;
        }
        
        /* Set background flag only on the last command */
        if (i == pipe_count - 1) {
            cmd->background = background;
        }
        
        /* Link commands in pipeline */
        if (!first_cmd) {
            first_cmd = cmd;
        } else {
            prev_cmd->next = cmd;
        }
        prev_cmd = cmd;
    }
    
    free(input_copy);
    return first_cmd;
}

/* Free a command and all commands in its pipeline */
void free_command(command_t *cmd) {
    while (cmd) {
        command_t *next = cmd->next;
        
        /* Free argv array */
        if (cmd->argv) {
            for (int i = 0; cmd->argv[i] != NULL; i++) {
                free(cmd->argv[i]);
            }
            free(cmd->argv);
        }
        
        /* Free redirection files */
        if (cmd->input_file) free(cmd->input_file);
        if (cmd->output_file) free(cmd->output_file);
        
        free(cmd);
        cmd = next;
    }
}
