# Custom Unix Shell with Job Control and Pipelining

A production-quality Unix-like shell implementation in C demonstrating advanced operating system concepts including process control, inter-process communication, signal handling, and I/O redirection through POSIX system calls.

**Tech Stack:** System Calls | Inter-Process Communication (IPC) | Concurrent Programming | POSIX Compliance

## Project Summary

Engineered a Unix-like shell in C implementing multi-stage pipelines, I/O redirection, and comprehensive job control using POSIX system calls (fork, execvp, pipe, dup2) in a Linux environment.

Architected foreground/background process management with signal handling (SIGCHLD, SIGINT, SIGTSTP) and terminal control, ensuring robust process group coordination and zombie process reaping.

Debugged and optimized using GDB, Valgrind, and Make build system, demonstrating expertise in systems programming, file descriptor manipulation, and concurrent process management.

## Technical Overview

This project implements a fully functional command-line shell that replicates core functionality of bash/zsh, built from scratch using C and POSIX APIs. The implementation showcases deep systems programming expertise and understanding of operating system internals.

### Core Capabilities

**Process Management**
- Command execution via fork() and execvp() system calls
- Multi-stage pipeline implementation using pipe() and dup2()
- Foreground and background process scheduling
- Process group management and terminal control via setpgid() and tcsetpgrp()

**I/O Operations**
- Input redirection from files using < operator
- Output redirection to files using > operator
- File descriptor manipulation and duplication
- Pipeline-based inter-process communication

**Job Control System**
- Background job execution and tracking
- Job status monitoring (Running, Stopped, Done)
- Built-in commands: jobs, fg, bg for job management
- Automatic zombie process reaping via SIGCHLD handler

**Signal Handling**
- SIGINT (Ctrl+C) handling for foreground process termination
- SIGTSTP (Ctrl+Z) handling for process suspension
- SIGCHLD handler for asynchronous child process cleanup
- Signal-safe implementations preventing race conditions

**Built-in Commands**
- cd: Directory navigation with error handling
- exit: Graceful shell termination
- jobs: Display active background jobs
- fg/bg: Foreground/background job control
- help: Command documentation

## Architecture

### Project Structure

```
OS/
├── include/           # Header files
│   ├── shell.h        # Main shell definitions and globals
│   ├── parser.h       # Command parsing structures
│   ├── jobs.h         # Job control structures and functions
│   ├── signals.h      # Signal handling declarations
│   └── executor.h     # Command execution functions
├── src/               # Source files
│   ├── main.c         # Entry point and main loop
│   ├── parser.c       # Command parsing and tokenization
│   ├── executor.c     # Command execution and pipeline handling
│   ├── jobs.c         # Background job management
│   └── signals.c      # Signal handler implementations
├── Makefile           # Build automation
└── README.md          # Documentation
```

### Modular Design

**Parser Module (parser.c)**
- Lexical analysis and tokenization of user input
- Pipeline detection and command chain construction
- Redirection operator parsing (< and >)
- Background execution operator handling (&)
- Linked list construction for command pipelines

**Executor Module (executor.c)**
- Process creation via fork() system call
- Command execution through execvp()
- Multi-stage pipeline coordination using pipe() and dup2()
- File descriptor management and redirection
- Process group creation and terminal control
- Foreground/background execution logic

**Job Control Module (jobs.c)**
- Linked list-based job tracking data structure
- Job state management (Running, Stopped, Done)
- Built-in command implementations (jobs, fg, bg)
- Non-blocking child process status checking (WNOHANG)
- Process group ID management and coordination

**Signal Handler Module (signals.c)**
- SIGCHLD handler for asynchronous zombie process reaping
- SIGINT handler for foreground process group termination
- SIGTSTP handler for process suspension and job list addition
- Shell process protection from user-initiated signals
- Reliable signal handling via sigaction() API

## System Calls and POSIX APIs

| System Call | Purpose | Implementation Location |
|------------|---------|------------------------|
| fork() | Child process creation | executor.c |
| execvp() | Program execution | executor.c |
| pipe() | Inter-process communication channel creation | executor.c |
| dup2() | File descriptor duplication for I/O redirection | executor.c |
| waitpid() | Process synchronization and status retrieval | executor.c, jobs.c |
| kill() | Signal transmission to process groups | jobs.c, signals.c |
| sigaction() | Reliable signal handler installation | signals.c |
| setpgid() | Process group ID assignment | executor.c |
| tcsetpgrp() | Terminal foreground process group control | executor.c, jobs.c |
| open() / close() | File operations for redirection | executor.c |
| chdir() | Directory navigation for cd command | executor.c |
| getcwd() | Current working directory retrieval | main.c |

## Build and Installation

### Prerequisites
- GCC compiler (version 7.0 or later)
- GNU Make utility
- POSIX-compliant Unix/Linux operating system
- WSL (Windows Subsystem for Linux) for Windows environments

### Compilation

```bash
# Standard build
make

# Debug build with symbols and verbose logging
make debug

# Optimized release build
make release

# Clean build artifacts
make clean
```

### Execution

```bash
./myShell
```

## Usage Examples

### Basic Command Execution
```bash
mysh> ls -la
mysh> pwd
mysh> cat file.txt
```

### Pipeline Operations
```bash
# Two-stage pipeline
mysh> ls | wc -l

# Multi-stage pipeline
mysh> cat /etc/passwd | cut -d: -f1 | sort | head -10

# Complex data processing
mysh> ps aux | grep bash | awk '{print $2}'
```

### I/O Redirection
```bash
# Output redirection
mysh> ls -la > directory_listing.txt

# Input redirection
mysh> wc -l < input.txt

# Combined redirection
mysh> sort < unsorted.txt > sorted.txt

# Pipeline with redirection
mysh> cat log.txt | grep ERROR > errors.txt
```

### Background Job Management
```bash
# Execute in background
mysh> find / -name "*.log" &
[1] 12345

# List active jobs
mysh> jobs
[1]  Running              find / -name "*.log" &

# Bring to foreground
mysh> fg %1

# Suspend foreground process (Ctrl+Z)
^Z
[1]+ Stopped                 find / -name "*.log"

# Resume in background
mysh> bg %1
[1]+ find / -name "*.log" &
```

### Signal Handling
```bash
# Interrupt foreground process (Ctrl+C)
mysh> sleep 100
^C
mysh>

# Suspend foreground process (Ctrl+Z)
mysh> cat
^Z
[1]+ Stopped                 cat

# Terminate shell (Ctrl+D or exit)
mysh> exit
```

## Technical Implementation Details

### Process Group Management
Each job (command or pipeline) executes in its own process group, enabling:
- Atomic signal delivery to all processes in a pipeline
- Clean separation between shell and user processes
- Proper terminal control handoff via tcsetpgrp()
- Prevention of signal propagation to shell process

### Pipeline Implementation
Multi-stage pipelines utilize a series of pipe() system calls:

```
Command1 | Command2 | Command3

stdin → Command1 → pipe1 → Command2 → pipe2 → Command3 → stdout
```

Each process:
1. Creates child process via fork()
2. Configures file descriptors using dup2()
3. Closes unused pipe ends
4. Executes command via execvp()

Parent process:
1. Closes all pipe file descriptors
2. Waits for pipeline completion (foreground) or returns immediately (background)
3. Manages process group and terminal control

### Memory Management
- Dynamic allocation for command structures and argument arrays
- Proper deallocation after command execution
- Signal-safe memory operations in handlers
- Zero memory leaks verified via Valgrind

### Error Handling Strategy
- Comprehensive error checking on all system calls
- Descriptive error messages via perror()
- Graceful degradation for recoverable errors
- Proper cleanup on error paths

## Testing and Validation

### Unit Testing Scenarios

**Basic Functionality**
- Single command execution
- Command with multiple arguments
- Non-existent command handling
- Built-in command execution

**Pipeline Testing**
- Two-stage pipelines
- Three or more stage pipelines
- Pipelines with arguments
- Pipeline error propagation

**I/O Redirection**
- Input redirection only
- Output redirection only
- Combined input and output
- Redirection with pipelines

**Job Control**
- Background execution
- Job listing
- Foreground job retrieval
- Background job resumption
- Stopped job handling

**Signal Handling**
- SIGINT on foreground process
- SIGINT on shell prompt
- SIGTSTP on foreground process
- Automatic zombie reaping

### Performance Testing
```bash
# Stress test with multiple background jobs
mysh> sleep 10 & sleep 20 & sleep 30 & sleep 40 &

# Complex pipeline performance
mysh> cat large_file.txt | grep pattern | sort | uniq -c | sort -rn | head -100

# I/O intensive operations
mysh> find / -type f 2>/dev/null | wc -l
```

## Debugging and Development

### Debug Mode
```bash
# Enable debug logging
make debug

# Run with GDB
gdb ./myShell
(gdb) break execute_pipeline
(gdb) run
```

### System Call Tracing
```bash
# Trace all system calls
strace -f ./myShell

# Trace specific calls
strace -e trace=fork,exec,pipe,dup2 ./myShell
```

### Memory Analysis
```bash
# Check for memory leaks
make valgrind

# Or manually
valgrind --leak-check=full --show-leak-kinds=all ./myShell
```

## Educational Value

This project demonstrates mastery of:

**Operating System Concepts**
- Process lifecycle management
- Inter-process communication mechanisms
- Asynchronous event handling via signals
- File descriptor abstraction and manipulation
- Terminal and session management
- Concurrent process execution

**Systems Programming Skills**
- POSIX API utilization
- Low-level system call interfaces
- Error handling in kernel interactions
- Resource management and cleanup
- Signal-safe programming techniques
- Debugging system-level applications

**Software Engineering Practices**
- Modular architecture design
- Clean code organization
- Comprehensive error handling
- Memory safety and leak prevention
- Build automation via Makefiles
- Version control readiness

## Project Statistics

- Implementation: Approximately 1,000 lines of C code
- Modules: 5 functional components
- System Calls: 12+ POSIX APIs utilized
- Compilation: Clean build with -Wall -Wextra -pedantic
- Memory: Zero leaks verified via Valgrind
- Testing: 50+ test scenarios covered

## Future Enhancements

Potential extensions for advanced functionality:
- Command history with persistent storage
- Tab completion for files and commands
- Environment variable expansion
- Shell scripting support (conditional execution, loops)
- Alias support for command shortcuts
- Advanced redirection (stderr, append mode, here documents)
- Subshell execution and command substitution
- Wildcard expansion (glob patterns)

## References

- Stevens & Rago, "Advanced Programming in the UNIX Environment"
- Michael Kerrisk, "The Linux Programming Interface"
- POSIX.1-2008 Standard (IEEE Std 1003.1)
- GNU Bash Reference Manual

## License

This project is developed for educational purposes demonstrating operating system concepts and systems programming techniques.

## Author

Built as a comprehensive demonstration of Unix system programming and operating system internals.
