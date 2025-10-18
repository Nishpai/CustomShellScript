# Design Document: myShell Architecture

## Overview
myShell is a Unix-like shell implementation that demonstrates comprehensive understanding of operating system concepts through practical implementation.

## System Architecture

### 1. Modular Design

```
┌─────────────────────────────────────────┐
│           User Interface                │
│      (Terminal I/O + Prompt)            │
└──────────────┬──────────────────────────┘
               │
┌──────────────▼──────────────────────────┐
│         Main Shell Loop                 │
│    (Input Reading + Dispatch)           │
└──────────────┬──────────────────────────┘
               │
       ┌───────┴────────┐
       │                │
┌──────▼──────┐  ┌─────▼──────┐
│   Parser    │  │  Executor  │
│  Module     │  │   Module   │
└──────┬──────┘  └─────┬──────┘
       │                │
       │         ┌──────▼──────────┐
       │         │  Job Control    │
       │         │     Module      │
       │         └──────┬──────────┘
       │                │
┌──────▼────────────────▼──────┐
│     Signal Handlers          │
│  (SIGCHLD, SIGINT, SIGTSTP)  │
└──────────────────────────────┘
```

### 2. Data Structures

#### Command Structure
```c
typedef struct command_t {
    char **argv;              // Command arguments
    char *input_file;         // Input redirection
    char *output_file;        // Output redirection
    int background;           // Background flag
    struct command_t *next;   // Pipeline link
} command_t;
```

**Purpose**: Represents a single command with all its attributes
**Memory**: Dynamically allocated, freed after execution
**Linking**: Forms a linked list for pipelines

#### Job Structure
```c
typedef struct job_t {
    int id;                   // User-visible job ID
    pid_t pid;                // Process ID
    pid_t pgid;               // Process group ID
    char *command;            // Command string
    job_status_t status;      // Running/Stopped/Done
    struct job_t *next;       // Next job
} job_t;
```

**Purpose**: Tracks background and stopped jobs
**Storage**: Linked list managed by jobs module
**Lifecycle**: Created on background/stop, removed on completion

### 3. Execution Flow

#### A. Simple Command
```
Input: "ls -la"
  ↓
Parse: argv = ["ls", "-la", NULL]
  ↓
Fork: Create child process
  ↓
Child: execvp("ls", argv)
  ↓
Parent: waitpid(child)
  ↓
Return to prompt
```

#### B. Pipeline
```
Input: "ls | grep txt | wc -l"
  ↓
Parse: 3 commands linked
  ↓
Create 2 pipes: pipe1, pipe2
  ↓
Fork 3 children:
  Child1: stdout → pipe1[write]
  Child2: stdin ← pipe1[read], stdout → pipe2[write]
  Child3: stdin ← pipe2[read]
  ↓
Parent: Close all pipe ends, waitpid() all children
  ↓
Return to prompt
```

#### C. Background Job
```
Input: "sleep 30 &"
  ↓
Parse: background = 1
  ↓
Fork: Create child process
  ↓
Child: setpgid(0,0), execvp("sleep", ...)
  ↓
Parent: Add to job list, print "[1] 12345"
  ↓
Return to prompt immediately (no waitpid)
  ↓
SIGCHLD handler reaps when complete
```

### 4. Process Group Management

```
Shell Process
  pid: 1000
  pgid: 1000
  Foreground: YES
  ↓
Launches: ls | grep txt
  ↓
  Process Group 1001:
    - ls (pid: 1001, pgid: 1001)
    - grep (pid: 1002, pgid: 1001)
  ↓
  tcsetpgrp(0, 1001) → Give terminal to job
  waitpid(-1001, ...) → Wait for group
  tcsetpgrp(0, 1000) → Return terminal to shell
```

**Key Points**:
- Each job gets its own process group (pgid)
- Terminal control passed via `tcsetpgrp()`
- Signals sent to entire groups using negative PIDs
- Background jobs don't get terminal control

### 5. Signal Handling Strategy

#### SIGCHLD (Child Terminated/Stopped)
```c
void sigchld_handler(int sig) {
    // Reap all zombie processes
    while (waitpid(-1, &status, WNOHANG|WUNTRACED) > 0) {
        // Update job status or remove completed jobs
    }
}
```
**Purpose**: Automatic cleanup of terminated children
**Flags**: WNOHANG (non-blocking), WUNTRACED (catch stops)

#### SIGINT (Ctrl+C)
```c
void sigint_handler(int sig) {
    if (fg_pid > 0) {
        kill(-fg_pid, SIGINT);  // Forward to foreground group
    }
    // Shell ignores - stays alive
}
```
**Purpose**: Terminate foreground job without killing shell

#### SIGTSTP (Ctrl+Z)
```c
void sigtstp_handler(int sig) {
    if (fg_pid > 0) {
        kill(-fg_pid, SIGTSTP);  // Stop foreground group
        // Job added to job list by waitpid in main executor
    }
}
```
**Purpose**: Stop foreground job and add to job list

### 6. I/O Redirection Implementation

#### Output Redirection: `cmd > file`
```c
int fd = open("file", O_WRONLY|O_CREAT|O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);  // Make stdout point to file
close(fd);
execvp(...);  // Command output goes to file
```

#### Input Redirection: `cmd < file`
```c
int fd = open("file", O_RDONLY);
dup2(fd, STDIN_FILENO);  // Make stdin read from file
close(fd);
execvp(...);  // Command reads from file
```

#### Pipeline: `cmd1 | cmd2`
```c
int pipefd[2];
pipe(pipefd);  // Create pipe

// Child 1 (cmd1)
dup2(pipefd[1], STDOUT_FILENO);  // stdout to pipe write end
close(pipefd[0]);
close(pipefd[1]);
execvp(cmd1...);

// Child 2 (cmd2)
dup2(pipefd[0], STDIN_FILENO);   // stdin from pipe read end
close(pipefd[0]);
close(pipefd[1]);
execvp(cmd2...);

// Parent
close(pipefd[0]);
close(pipefd[1]);
waitpid(child1); waitpid(child2);
```

### 7. Memory Management

#### Allocation Points
1. **Command parsing**: `malloc()` for argv, command_t structures
2. **Job tracking**: `malloc()` for job_t, strdup() for command strings
3. **Input reading**: `malloc()` for input buffer

#### Deallocation Strategy
1. **Commands**: `free_command()` after execution
2. **Jobs**: Remove and free when completed
3. **Input**: Free after parsing

#### Memory Safety
- All allocations checked for NULL
- Proper cleanup in signal handlers
- No memory leaks (verified with valgrind)

### 8. Error Handling

#### System Call Failures
```c
if (fork() < 0) {
    perror("fork");
    return -1;
}
```

#### Command Not Found
```c
execvp(...);
// If reached, execvp failed
fprintf(stderr, "command not found\n");
exit(EXIT_FAILURE);
```

#### File Redirection Errors
```c
int fd = open(file, ...);
if (fd < 0) {
    perror(file);
    exit(EXIT_FAILURE);
}
```

### 9. Concurrency Model

```
Shell (single-threaded)
  ↓
Fork children for each command
  ↓
Children run concurrently:
  - Pipeline processes run simultaneously
  - Background jobs run while shell continues
  - Signal handlers run asynchronously
  ↓
Shell coordinates via:
  - waitpid() for synchronization
  - Process groups for control
  - Signals for communication
```

### 10. Testing Strategy

#### Unit Testing Approach
1. **Parser**: Test with various input patterns
2. **Executor**: Test single commands, pipes, redirections
3. **Jobs**: Test add/remove/update operations
4. **Signals**: Test Ctrl+C, Ctrl+Z behavior

#### Integration Testing
1. **End-to-end**: Full command execution paths
2. **Error scenarios**: Invalid commands, missing files
3. **Stress testing**: Many concurrent jobs
4. **Signal handling**: Interrupt combinations

#### Validation Tools
- `valgrind`: Memory leak detection
- `strace`: System call verification
- `gdb`: Interactive debugging
- Manual testing: User interaction scenarios

## Performance Considerations

### Optimization Strategies
1. **Minimal allocations**: Reuse buffers where possible
2. **Efficient parsing**: Single-pass tokenization
3. **Lazy cleanup**: Jobs cleaned only when needed
4. **Process groups**: Efficient signal delivery

### Scalability Limits
- **Max jobs**: Limited by MAX_JOBS constant
- **Command length**: Limited by MAX_LINE constant
- **Pipeline stages**: Limited by file descriptor table
- **Concurrent processes**: Limited by system resources

## Security Considerations

### Implemented Safeguards
1. **No shell injection**: Direct execvp(), no system()
2. **File permissions**: Proper modes on created files
3. **Signal isolation**: Shell protected from user signals
4. **Resource limits**: Inherits from parent shell

### Known Limitations
1. **No authentication**: Runs with user privileges
2. **No sandboxing**: Commands have full user access
3. **No input validation**: Trusts user input
4. **Environment inheritance**: Uses parent environment

## Future Architecture Extensions

### Command History
```
Add: history.c module
  - Circular buffer for commands
  - File persistence (~/.mysh_history)
  - Up/down arrow handling
```

### Tab Completion
```
Add: completion.c module
  - Readline library integration
  - Path completion
  - Command completion
```

### Scripting Support
```
Add: script.c module
  - File parsing
  - Variable substitution
  - Control flow (if/while)
```

## Conclusion

This architecture demonstrates:
- Clean modular design
- Proper separation of concerns
- Robust error handling
- Efficient process management
- Educational clarity

The implementation serves as a practical demonstration of core OS concepts while maintaining production-quality code structure.
