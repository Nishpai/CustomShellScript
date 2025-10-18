#ifndef SIGNALS_H
#define SIGNALS_H

#include "shell.h"

/* Signal Handler Functions */
void setup_signal_handlers(void);
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

#endif /* SIGNALS_H */
