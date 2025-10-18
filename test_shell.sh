#!/bin/bash
# Test script for myShell
# This script creates test files and provides test scenarios

echo "=================================="
echo "myShell Test Environment Setup"
echo "=================================="

# Create test directory
TEST_DIR="/tmp/myshell_test"
mkdir -p "$TEST_DIR"
cd "$TEST_DIR"

# Create test files
echo "Creating test files..."

# Create sample text files
echo "apple" > fruits.txt
echo "banana" >> fruits.txt
echo "cherry" >> fruits.txt
echo "orange" >> fruits.txt

echo "error: file not found" > log1.txt
echo "info: system started" >> log1.txt
echo "error: connection failed" >> log1.txt
echo "warning: low memory" >> log1.txt

echo "one two three" > numbers.txt
echo "four five six" >> numbers.txt
echo "seven eight nine" >> numbers.txt

cat > unsorted.txt << EOF
zebra
apple
mango
banana
EOF

# Create a simple C program for testing
cat > hello.c << 'EOF'
#include <stdio.h>
int main() {
    printf("Hello from compiled program!\n");
    return 0;
}
EOF

# Compile if gcc is available
if command -v gcc &> /dev/null; then
    gcc hello.c -o hello
    echo "✓ Compiled hello.c"
fi

echo ""
echo "Test files created in: $TEST_DIR"
echo ""

# Display test scenarios
cat << 'EOF'

================================
TEST SCENARIOS FOR myShell
================================

1. BASIC COMMANDS
   ------------------
   pwd
   ls
   ls -la
   echo "Hello World"
   date
   whoami


2. PIPELINES
   ------------------
   ls | wc -l
   cat fruits.txt | sort
   cat log1.txt | grep error
   ps aux | grep bash | wc -l
   cat numbers.txt | wc -w


3. I/O REDIRECTION
   ------------------
   cat < fruits.txt
   echo "test" > output.txt
   cat output.txt
   sort < unsorted.txt > sorted.txt
   cat sorted.txt
   cat log1.txt | grep error > errors_only.txt
   

4. COMPLEX PIPELINES
   ------------------
   cat fruits.txt | sort | head -2
   ls -la | grep txt | wc -l
   cat /etc/passwd | cut -d: -f1 | sort | head -5


5. BACKGROUND JOBS
   ------------------
   sleep 5 &
   jobs
   sleep 10 &
   sleep 15 &
   jobs
   

6. JOB CONTROL
   ------------------
   Step 1: sleep 100
   Step 2: Press Ctrl+Z
   Step 3: jobs
   Step 4: bg %1
   Step 5: jobs
   Step 6: fg %1
   Step 7: Press Ctrl+C


7. SIGNAL HANDLING
   ------------------
   # Test Ctrl+C
   sleep 30
   [Press Ctrl+C - should kill sleep, not shell]
   
   # Test Ctrl+Z
   cat
   [Type something, then press Ctrl+Z]
   jobs
   fg %1
   [Press Ctrl+C to exit cat]


8. MIXED OPERATIONS
   ------------------
   cat fruits.txt | sort > sorted_fruits.txt &
   jobs
   cat < numbers.txt | wc -w > word_count.txt
   cat word_count.txt


9. BUILT-IN COMMANDS
   ------------------
   cd /tmp
   pwd
   cd -
   cd ~
   pwd
   help
   jobs


10. ERROR HANDLING
    ------------------
    nonexistent_command
    cat nonexistent_file.txt
    cd /nonexistent_directory
    ls | fakecmd


11. STRESS TEST
    ------------------
    sleep 2 & sleep 3 & sleep 4 & sleep 5 &
    jobs
    # Wait a few seconds
    jobs
    # All should complete

12. COMPLEX REDIRECTION
    ------------------
    cat < log1.txt | grep error > errors.txt
    cat errors.txt
    sort < unsorted.txt | head -2 > top2.txt
    cat top2.txt

================================
VERIFICATION COMMANDS
================================

After running tests, verify with:
  - ls -la
  - cat output.txt
  - cat sorted.txt
  - cat errors_only.txt
  - jobs (should show running/stopped jobs)

================================
CLEANUP
================================

To remove test files:
  rm -rf /tmp/myshell_test

EOF

echo ""
echo "Test environment ready!"
echo "Navigate to: cd $TEST_DIR"
echo "Then run: /path/to/myShell"
echo ""
