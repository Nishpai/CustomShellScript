# Building myShell on Windows

## ⚠️ Important Note

**myShell is designed for Unix/Linux systems** and uses POSIX system calls that are not natively available on Windows. However, you have several options to build and run it on a Windows machine.

## Option 1: WSL (Windows Subsystem for Linux) - **RECOMMENDED**

### Step 1: Install WSL
```powershell
# In PowerShell (as Administrator)
wsl --install
```

Or install a specific distribution:
```powershell
wsl --install -d Ubuntu
```

### Step 2: Open WSL Terminal
- Search for "Ubuntu" or "WSL" in Start menu
- Or run `wsl` from PowerShell

### Step 3: Install Build Tools
```bash
sudo apt update
sudo apt install build-essential gcc make
```

### Step 4: Navigate to Project
```bash
# Your Windows drives are mounted under /mnt/
cd /mnt/c/Users/nishp/OneDrive/Desktop/Projects/OS
```

### Step 5: Build and Run
```bash
make
./myShell
```

## Option 2: Git Bash (Limited Support)

Git Bash provides a Unix-like environment but **may have limitations** with process control and signals.

### Installation
1. Download Git for Windows: https://git-scm.com/download/win
2. Install with default options
3. Open Git Bash

### Build
```bash
cd /c/Users/nishp/OneDrive/Desktop/Projects/OS
make
./myShell
```

⚠️ **Note**: Some features like job control may not work properly in Git Bash.

## Option 3: Cygwin (Alternative)

Cygwin provides a more complete POSIX environment.

### Installation
1. Download Cygwin installer: https://www.cygwin.com/
2. Install with gcc, make, and development packages
3. Open Cygwin terminal

### Build
```bash
cd /cygdrive/c/Users/nishp/OneDrive/Desktop/Projects/OS
make
./myShell
```

## Option 4: Linux Virtual Machine

### Using VirtualBox or VMware
1. Install VirtualBox/VMware
2. Install Ubuntu or another Linux distribution
3. Copy project files to VM
4. Build normally

## Option 5: Docker (Advanced)

### Create Dockerfile
```dockerfile
FROM ubuntu:latest
RUN apt-get update && apt-get install -y gcc make
WORKDIR /app
COPY . .
RUN make
CMD ["./myShell"]
```

### Build and Run
```powershell
docker build -t myshell .
docker run -it myshell
```

## 🎯 Recommended Workflow

### For Development on Windows:

1. **Use WSL2** (best compatibility)
   ```bash
   # In WSL
   cd /mnt/c/Users/nishp/OneDrive/Desktop/Projects/OS
   make
   ./myShell
   ```

2. **Edit Code in VS Code**
   - Install "Remote - WSL" extension
   - Open folder in WSL: `code .`
   - Edit files in Windows, build in WSL

3. **Version Control**
   ```bash
   # In WSL or PowerShell
   git init
   git add .
   git commit -m "Initial commit"
   ```

## 🔧 Troubleshooting Windows-Specific Issues

### Issue: "Permission denied" when running ./myShell
```bash
# Make executable
chmod +x myShell
```

### Issue: Line ending problems (CRLF vs LF)
```bash
# Convert to Unix line endings
sudo apt install dos2unix
find . -name "*.c" -o -name "*.h" | xargs dos2unix
```

### Issue: Can't find gcc
```bash
# Install build tools
sudo apt update
sudo apt install build-essential
```

### Issue: Make command not found
```bash
# Install make
sudo apt install make
```

### Issue: Path with spaces
```bash
# Use quotes
cd "/mnt/c/Users/nishp/OneDrive/Desktop/Projects/OS"
```

## 📋 Quick Setup Checklist (WSL)

- [ ] WSL installed (`wsl --install`)
- [ ] Ubuntu/Linux distribution installed
- [ ] Build tools installed (`sudo apt install build-essential`)
- [ ] Navigated to project directory
- [ ] Run `make` successfully
- [ ] Run `./myShell` successfully

## 🖥️ VS Code Setup for WSL

### Install Extensions
1. Remote - WSL
2. C/C++
3. C/C++ Extension Pack

### Open Project in WSL
```bash
# In WSL terminal
cd /mnt/c/Users/nishp/OneDrive/Desktop/Projects/OS
code .
```

This opens VS Code with WSL backend - you get Windows UI with Linux compilation!

### Configure IntelliSense
VS Code should auto-detect C configuration in WSL. If not, create `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "WSL",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c11",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```

## 🚀 First Time Setup Script (PowerShell)

```powershell
# Run in PowerShell as Administrator
Write-Host "Setting up myShell development environment..."

# Install WSL if not already installed
if (!(Get-Command wsl -ErrorAction SilentlyContinue)) {
    Write-Host "Installing WSL..."
    wsl --install
    Write-Host "Please restart your computer and run this script again."
    exit
}

# Start WSL and install tools
wsl bash -c "sudo apt update && sudo apt install -y build-essential gcc make"

Write-Host "Setup complete!"
Write-Host "To build the project:"
Write-Host "1. Open WSL: wsl"
Write-Host "2. Navigate: cd /mnt/c/Users/nishp/OneDrive/Desktop/Projects/OS"
Write-Host "3. Build: make"
Write-Host "4. Run: ./myShell"
```

## 📝 Notes

1. **WSL is the best option** - Full Linux compatibility
2. **Path format**: Windows `C:\` becomes `/mnt/c/` in WSL
3. **File permissions**: Set in WSL, not Windows
4. **Performance**: Native Linux performance in WSL2
5. **Networking**: Works seamlessly between Windows and WSL

## ✅ Verification

After setup, verify everything works:

```bash
# Check gcc
gcc --version

# Check make
make --version

# Build project
cd /mnt/c/Users/nishp/OneDrive/Desktop/Projects/OS
make clean
make

# Should see:
# Compiling: src/main.c
# Compiling: src/parser.c
# ...
# Build complete: ./myShell

# Run shell
./myShell

# You should see the welcome screen!
```

## 🆘 Getting Help

If you encounter issues:
1. Ensure WSL2 is installed (not WSL1)
2. Update WSL: `wsl --update`
3. Check Ubuntu is latest: `sudo apt update && sudo apt upgrade`
4. Verify you're in the correct directory: `pwd`
5. Check file permissions: `ls -la`

## 🎓 Learning Path

1. **Start with WSL**: Most compatible
2. **Use VS Code**: Best IDE integration
3. **Learn WSL basics**: Essential for development
4. **Experiment**: Try building in different environments
5. **Compare**: See differences between environments

---

**Bottom Line**: Use WSL2 on Windows for the best experience! It provides a genuine Linux environment without the overhead of a full VM.
