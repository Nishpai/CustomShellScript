# Quick Start Guide - myShell

## ⚡ 5-Minute Setup

### 1. Build the Shell
```bash
cd OS
make
```

### 2. Run the Shell
```bash
./myShell
```

### 3. Try These Commands

```bash
# Basic command
mysh> ls

# Pipeline
mysh> ls | wc -l

# Redirection
mysh> echo "Hello" > test.txt
mysh> cat test.txt

# Background job
mysh> sleep 10 &
mysh> jobs

# Get help
mysh> help

# Exit
mysh> exit
```

## 📋 Common Operations

### Navigate Directories
```bash
mysh> pwd              # Show current directory
mysh> cd /tmp          # Change to /tmp
mysh> cd ..            # Go up one level
mysh> cd ~             # Go to home directory
```

### Work with Files
```bash
mysh> cat file.txt                      # Display file
mysh> cat file.txt | grep "pattern"     # Search in file
mysh> ls *.txt > filelist.txt           # Save file list
mysh> wc -l < input.txt                 # Count lines
```

### Manage Jobs
```bash
mysh> find / -name "*.log" &            # Start in background
[1] 12345
mysh> jobs                               # List all jobs
[1]  Running              find / -name "*.log" &

mysh> sleep 100                          # Start foreground
^Z                                       # Press Ctrl+Z to stop
[2]+ Stopped                 sleep 100

mysh> bg %2                              # Resume in background
mysh> fg %2                              # Bring to foreground
^C                                       # Press Ctrl+C to kill
```

### Pipeline Examples
```bash
# Count files
mysh> ls | wc -l

# Find and count
mysh> ps aux | grep bash | wc -l

# Sort and display top
mysh> cat file.txt | sort | head -10

# Complex pipeline
mysh> cat /etc/passwd | cut -d: -f1 | sort | uniq | wc -l
```

## 🔧 Troubleshooting

### Build Errors
```bash
# Clean and rebuild
make clean
make

# Check GCC version
gcc --version

# Ensure you're on Linux/Unix
uname -s
```

### Runtime Issues
```bash
# Check permissions
chmod +x myShell

# Run from correct directory
cd /path/to/OS
./myShell

# If job control issues, ensure running in terminal
# (not in background or non-interactive mode)
```

## 🎯 Test Scenarios

### Quick Test Suite
```bash
# 1. Basic
mysh> pwd
mysh> ls
mysh> echo "test"

# 2. Pipes
mysh> ls | wc -l
mysh> cat /etc/passwd | head -5

# 3. Redirection
mysh> echo "line1" > test.txt
mysh> echo "line2" >> test.txt
mysh> cat < test.txt

# 4. Background
mysh> sleep 5 &
mysh> jobs
# Wait 5 seconds
mysh> jobs

# 5. Job Control
mysh> sleep 30
# Press Ctrl+Z
mysh> jobs
mysh> bg %1
mysh> fg %1
# Press Ctrl+C
```

## 📚 Learn More

- Full documentation: `README.md`
- Architecture details: `DESIGN.md`
- Test scenarios: `test_shell.sh`

## ❓ FAQ

**Q: Does it work on Windows?**
A: You need WSL (Windows Subsystem for Linux) or a Linux VM.

**Q: Why does Ctrl+C kill my shell?**
A: Make sure you compiled correctly. The shell should ignore Ctrl+C when no foreground job is running.

**Q: Jobs command shows nothing?**
A: Only background and stopped jobs appear. Completed jobs are automatically removed.

**Q: Can I use it as my default shell?**
A: Not recommended for daily use. This is an educational project.

**Q: How do I debug issues?**
A: Use `make debug` to build with debug symbols and enable logging.

## 🚀 Next Steps

1. Read `README.md` for complete feature list
2. Run `test_shell.sh` to set up test environment
3. Review `DESIGN.md` to understand architecture
4. Experiment with complex commands
5. Try breaking it (safely!) to understand error handling

Enjoy exploring Unix internals! 🐧
