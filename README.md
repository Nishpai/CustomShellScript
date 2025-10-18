# myShell - Custom Unix-Like Shell with Job Control & Pipelining

A fully-featured Unix-like shell implementation in C that demonstrates deep understanding of operating system concepts including process control, inter-process communication, signal handling, and I/O redirection.

## 🎯 Features

### Core Functionality
- ✅ **Command Execution**: Execute standard Unix commands (`ls`, `cat`, `grep`, etc.)
- ✅ **Pipelining**: Chain multiple commands together (`ls | grep txt | wc -l`)
- ✅ **I/O Redirection**: Redirect input and output (`cat < input.txt > output.txt`)
- ✅ **Background Jobs**: Run commands in background (`sleep 10 &`)
- ✅ **Job Control**: Manage background and foreground jobs (`jobs`, `fg`, `bg`)
- ✅ **Signal Handling**: Proper handling of Ctrl+C and Ctrl+Z
- ✅ **Built-in Commands**: `cd`, `exit`, `jobs`, `fg`, `bg`, `help`
- ✅ **Process Groups**: Proper terminal control and process group management
- ✅ **Colorized Prompt**: User-friendly prompt showing username, hostname, and current directory

## 🏗️ Architecture

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
└── README.md          # This file
```

### Key Components

#### 1. Parser Module (`parser.c`)
- Tokenizes user input into command structures
- Handles pipe (`|`) detection and splitting
- Processes I/O redirection (`<`, `>`)
- Detects background operator (`&`)
- Builds linked list of command structures for pipelines

#### 2. Executor Module (`executor.c`)
- Uses `fork()` to create child processes
- Uses `execvp()` to execute commands
- Implements pipeline execution with `pipe()` and `dup2()`
- Manages file descriptor redirection
- Handles process groups with `setpgid()` and `tcsetpgrp()`
- Distinguishes between foreground and background execution

#### 3. Job Control Module (`jobs.c`)
- Maintains linked list of background jobs
- Tracks job status (Running, Stopped, Done)
- Implements `jobs`, `fg`, and `bg` built-in commands
- Cleans up completed jobs with `waitpid(WNOHANG)`
- Manages job IDs and process group IDs

#### 4. Signal Handler Module (`signals.c`)
- **SIGCHLD**: Reaps zombie processes automatically
- **SIGINT** (Ctrl+C): Forwards to foreground process group only
- **SIGTSTP** (Ctrl+Z): Stops foreground process and adds to job list
- Prevents shell from being terminated by signals
- Uses `sigaction()` for reliable signal handling

## 🔧 System Calls Demonstrated

| System Call | Usage |
|------------|-------|
| `fork()` | Create child processes |
| `execvp()` | Execute commands |
| `pipe()` | Create pipes for inter-process communication |
| `dup2()` | Duplicate file descriptors for I/O redirection |
| `waitpid()` | Wait for child process termination/status change |
| `kill()` | Send signals to processes |
| `signal()` / `sigaction()` | Install signal handlers |
| `setpgid()` | Set process group ID |
| `tcsetpgrp()` | Give terminal control to process group |
| `open()` / `close()` | File operations for redirection |
| `chdir()` | Change directory (cd command) |

## 📦 Installation & Compilation

### Prerequisites
- GCC compiler
- Linux/Unix operating system (or WSL on Windows)
- Make utility

### Build Instructions

```bash
# Clone or navigate to project directory
cd OS

# Build the shell
make

# Or use specific build targets:
make all      # Standard build
make debug    # Build with debug symbols
make release  # Optimized release build
```

### Clean Build
```bash
make clean    # Remove build artifacts
make rebuild  # Clean and rebuild
```

## 🚀 Usage

### Starting the Shell
```bash
./myShell
```

You'll see a welcome message and prompt:
```
╔════════════════════════════════════════════╗
║     myShell - Custom Unix-like Shell      ║
║   With Job Control & Pipelining Support   ║
╚════════════════════════════════════════════╝

Type 'help' for available commands.
Type 'exit' to quit.

[user@hostname:~/projects] mysh> 
```

### Basic Commands
```bash
# Regular command execution
mysh> ls -la
mysh> pwd
mysh> cat file.txt

# Change directory
mysh> cd /tmp
mysh> cd ..
mysh> cd ~

# Exit shell
mysh> exit
```

### Pipelines
```bash
# Simple pipeline
mysh> ls | grep txt

# Multi-stage pipeline
mysh> ps aux | grep bash | wc -l

# Complex pipeline
mysh> cat /etc/passwd | cut -d: -f1 | sort | head -10
```

### I/O Redirection
```bash
# Output redirection
mysh> ls > files.txt
mysh> echo "Hello World" > output.txt

# Input redirection
mysh> cat < input.txt
mysh> wc -l < file.txt

# Both input and output
mysh> grep error < log.txt > errors.txt

# With pipelines
mysh> cat input.txt | grep pattern > output.txt
```

### Background Jobs
```bash
# Run in background
mysh> sleep 30 &
[1] 12345

# Long-running command
mysh> find / -name "*.log" > search.txt &

# Multiple background jobs
mysh> sleep 10 &
mysh> sleep 20 &
mysh> jobs
[1]  Running              sleep 10 &
[2]  Running              sleep 20 &
```

### Job Control
```bash
# List all jobs
mysh> jobs

# Bring job to foreground
mysh> fg %1

# Resume stopped job in background
mysh> bg %2

# Stop foreground job (Ctrl+Z)
mysh> sleep 100
^Z
[1]+ Stopped                 sleep 100

# Then resume it
mysh> bg %1
[1]+ sleep 100 &
```

### Signal Handling
```bash
# Ctrl+C - Terminate foreground process
mysh> sleep 100
^C
mysh> 

# Ctrl+Z - Stop foreground process
mysh> cat
^Z
[1]+ Stopped                 cat

# Ctrl+D - Exit shell (EOF)
mysh> [Ctrl+D]
```

## 🧪 Testing Scenarios

### Test Suite

#### Basic Functionality
| Test | Command | Expected Behavior |
|------|---------|-------------------|
| Simple command | `ls` | Lists directory contents |
| Command with args | `ls -la` | Lists with flags |
| Nonexistent command | `fakecmd` | Error: "command not found" |
| Built-in cd | `cd /tmp && pwd` | Changes directory |

#### Pipelines
| Test | Command | Expected Behavior |
|------|---------|-------------------|
| Two-stage pipe | `ls \| grep txt` | Filters ls output |
| Three-stage pipe | `cat file \| grep error \| wc -l` | Counts matching lines |
| Pipeline with args | `ps aux \| grep bash \| awk '{print $2}'` | Extracts PIDs |

#### I/O Redirection
| Test | Command | Expected Behavior |
|------|---------|-------------------|
| Output redirect | `echo "test" > file.txt` | Creates file with content |
| Input redirect | `cat < input.txt` | Reads from file |
| Both redirects | `sort < unsorted.txt > sorted.txt` | Sorts file |
| Pipe + redirect | `ls \| grep txt > results.txt` | Saves filtered output |

#### Background Jobs
| Test | Command | Expected Behavior |
|------|---------|-------------------|
| Background exec | `sleep 5 &` | Returns prompt immediately |
| Background pipeline | `find / -name "*.c" \| wc -l &` | Runs in background |
| Multiple bg jobs | `sleep 10 & sleep 20 &` | Both run concurrently |

#### Job Control
| Test | Command | Expected Behavior |
|------|---------|-------------------|
| List jobs | `jobs` | Shows all background/stopped jobs |
| Foreground job | `fg %1` | Brings job 1 to foreground |
| Background job | `bg %1` | Resumes job 1 in background |
| Stop and resume | `sleep 100` → Ctrl+Z → `bg %1` | Stops then resumes |

#### Signal Handling
| Test | Action | Expected Behavior |
|------|--------|-------------------|
| Ctrl+C on fg | Run `sleep 100`, press Ctrl+C | Terminates sleep, shell continues |
| Ctrl+C on shell | Press Ctrl+C at prompt | Shell ignores, stays active |
| Ctrl+Z on fg | Run `cat`, press Ctrl+Z | Stops cat, adds to jobs list |
| Ctrl+D | Press Ctrl+D | Shell exits gracefully |

### Manual Testing Script

Create a test file `test_commands.txt`:
```bash
# Basic commands
pwd
ls
echo "Testing myShell"

# Pipelines
ls | wc -l
cat /etc/passwd | head -5

# Redirection
echo "test output" > /tmp/test.txt
cat < /tmp/test.txt

# Background
sleep 3 &
jobs
```

## 🐛 Debugging

### Enable Debug Mode
```bash
# Build with debug symbols and logging
make debug

# Or manually enable in code
# Edit include/shell.h and change:
#define DEBUG 1
```

### Use GDB
```bash
gdb ./myShell
(gdb) run
(gdb) break execute_pipeline
(gdb) continue
```

### System Call Tracing
```bash
# Trace system calls (Linux only)
strace -f ./myShell

# Follow specific child processes
strace -f -e trace=fork,exec,pipe,dup2 ./myShell
```

### Memory Leak Detection
```bash
# Requires valgrind
make valgrind

# Or manually:
valgrind --leak-check=full --show-leak-kinds=all ./myShell
```

## 📚 Technical Details

### Process Group Management
- Shell creates its own process group on startup
- Each job (or pipeline) gets its own process group
- Terminal control is passed via `tcsetpgrp()`
- Signals are sent to entire process groups using negative PIDs

### Pipeline Implementation
```
Command1 | Command2 | Command3

[stdin] → Command1 → [pipe1] → Command2 → [pipe2] → Command3 → [stdout]
```

Each command:
1. Forks a child process
2. Sets up appropriate file descriptors using `dup2()`
3. Closes unused pipe ends
4. Executes command with `execvp()`

### Signal Flow
```
User presses Ctrl+C
    ↓
SIGINT sent to foreground process group
    ↓
Shell's SIGINT handler
    ↓
If fg_pid > 0: forward to process group
If fg_pid == 0: ignore (shell only)
```

## 🎓 Learning Outcomes

This project demonstrates mastery of:
- **Process Management**: Creating, monitoring, and controlling processes
- **Inter-Process Communication**: Pipes and file descriptors
- **Signal Handling**: Asynchronous event handling
- **File I/O**: File descriptor manipulation and redirection
- **Terminal Control**: Process groups and terminal foreground control
- **Memory Management**: Dynamic allocation and cleanup
- **System Programming**: Direct interaction with OS kernel
- **Concurrent Execution**: Managing multiple simultaneous processes

## 🔄 Future Enhancements

Potential improvements:
- [ ] Command history with up/down arrow keys (using `readline` library)
- [ ] Tab autocompletion for commands and files
- [ ] Shell scripting support (execute commands from file)
- [ ] Environment variable expansion (`$HOME`, `$PATH`)
- [ ] Command substitution (`` `cmd` `` or `$(cmd)`)
- [ ] Conditional execution (`&&`, `||`)
- [ ] Redirect stderr separately (`2>`)
- [ ] Append mode (`>>`)
- [ ] Here documents (`<<`)
- [ ] Wildcard expansion (`*.txt`)
- [ ] Aliases
- [ ] Configuration file (`~/.myshrc`)

## 📖 References

- Advanced Programming in the UNIX Environment (Stevens & Rago)
- The Linux Programming Interface (Michael Kerrisk)
- GNU Bash Manual
- POSIX.1-2008 Standard

## 📝 License

This project is created for educational purposes as part of an Operating Systems course.

## 👨‍💻 Author

Created as a comprehensive demonstration of Unix system programming concepts.

---

**Note**: This shell is designed for Unix-like systems (Linux, macOS, BSD). It requires POSIX-compliant system calls and will not work natively on Windows (use WSL or a Linux VM).
#   C u s t o m S h e l l S c r i p t  
 