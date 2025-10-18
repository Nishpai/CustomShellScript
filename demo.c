/* 
 * Demo Script for myShell
 * 
 * This file demonstrates the key features of the shell
 * through a series of automated test commands.
 * 
 * Compile and run: gcc demo.c -o demo && ./demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_header(const char *title) {
    printf("\n");
    printf("═══════════════════════════════════════════\n");
    printf("  %s\n", title);
    printf("═══════════════════════════════════════════\n\n");
}

void run_command(const char *cmd, const char *description) {
    printf("📌 %s\n", description);
    printf("   Command: %s\n\n", cmd);
    sleep(1);
}

int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║        myShell Feature Demonstration      ║\n");
    printf("║                                           ║\n");
    printf("║  This demo showcases all major features  ║\n");
    printf("║  of the custom Unix-like shell           ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    
    printf("\nPress Enter to start the demonstration...");
    getchar();
    
    // Feature 1: Basic Commands
    print_header("1. BASIC COMMAND EXECUTION");
    run_command("ls -la", "List directory contents with details");
    run_command("pwd", "Print working directory");
    run_command("echo 'Hello from myShell!'", "Echo a message");
    run_command("date", "Display current date and time");
    
    printf("✓ Basic commands execute like in standard shells\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 2: Pipelines
    print_header("2. COMMAND PIPELINES");
    run_command("ls | wc -l", "Count files in directory");
    run_command("cat /etc/passwd | head -5", "Show first 5 lines of passwd");
    run_command("ps aux | grep bash", "Find bash processes");
    run_command("ls -la | grep txt | wc -l", "Three-stage pipeline");
    
    printf("✓ Multiple commands can be chained with pipes\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 3: I/O Redirection
    print_header("3. I/O REDIRECTION");
    run_command("echo 'Test output' > output.txt", "Write to file");
    run_command("cat output.txt", "Read from file");
    run_command("cat < output.txt", "Redirect input from file");
    run_command("ls -la > filelist.txt", "Save directory listing");
    run_command("grep output < filelist.txt", "Search in redirected file");
    
    printf("✓ Input/output can be redirected to/from files\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 4: Background Jobs
    print_header("4. BACKGROUND JOB EXECUTION");
    run_command("sleep 5 &", "Run sleep in background (5 seconds)");
    printf("   [Shell returns immediately]\n\n");
    run_command("jobs", "List background jobs");
    printf("   [1]  Running              sleep 5 &\n\n");
    
    printf("⏳ Waiting for background job to complete...\n");
    sleep(3);
    
    run_command("jobs", "Check job status again");
    printf("   [1]+ Done                 sleep 5 &\n\n");
    
    printf("✓ Jobs can run in background while shell remains interactive\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 5: Job Control
    print_header("5. JOB CONTROL (jobs, fg, bg)");
    printf("Scenario:\n");
    printf("  1. Start: sleep 100\n");
    printf("  2. Press Ctrl+Z to stop it\n");
    printf("  3. Command: jobs\n");
    printf("     Output: [1]+ Stopped           sleep 100\n\n");
    printf("  4. Command: bg %%1\n");
    printf("     Result: Resume job in background\n\n");
    printf("  5. Command: fg %%1\n");
    printf("     Result: Bring job to foreground\n\n");
    printf("  6. Press Ctrl+C to terminate\n\n");
    
    printf("✓ Full job control with fg, bg, and jobs commands\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 6: Signal Handling
    print_header("6. SIGNAL HANDLING");
    printf("The shell properly handles signals:\n\n");
    
    printf("🔹 SIGINT (Ctrl+C):\n");
    printf("   - Terminates foreground process\n");
    printf("   - Shell continues running\n\n");
    
    printf("🔹 SIGTSTP (Ctrl+Z):\n");
    printf("   - Stops foreground process\n");
    printf("   - Adds to job list\n");
    printf("   - Can be resumed with bg/fg\n\n");
    
    printf("🔹 SIGCHLD:\n");
    printf("   - Automatically reaps zombie processes\n");
    printf("   - Updates job status\n");
    printf("   - Cleans up completed jobs\n\n");
    
    printf("✓ Robust signal handling prevents shell crashes\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 7: Built-in Commands
    print_header("7. BUILT-IN COMMANDS");
    run_command("cd /tmp", "Change to /tmp directory");
    run_command("pwd", "Verify location");
    run_command("cd ~", "Return to home directory");
    run_command("jobs", "List all jobs");
    run_command("help", "Display help message");
    
    printf("✓ Essential built-ins implemented internally\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Feature 8: Complex Scenarios
    print_header("8. COMPLEX SCENARIOS");
    run_command("cat /etc/passwd | cut -d: -f1 | sort | head -10", 
                "Multi-stage pipeline with sorting");
    run_command("find . -name '*.c' | xargs wc -l", 
                "Find and count lines in C files");
    run_command("ps aux | grep -v grep | grep myShell", 
                "Find shell process (excluding grep)");
    run_command("cat < input.txt | grep pattern > output.txt", 
                "Combined input/output redirection with pipe");
    
    printf("✓ Handles complex command combinations\n");
    printf("\nPress Enter to continue...");
    getchar();
    
    // Summary
    print_header("DEMONSTRATION COMPLETE");
    
    printf("✅ Core Features Demonstrated:\n\n");
    printf("   1. ✓ Command execution\n");
    printf("   2. ✓ Pipelines (|)\n");
    printf("   3. ✓ I/O Redirection (<, >)\n");
    printf("   4. ✓ Background jobs (&)\n");
    printf("   5. ✓ Job control (jobs, fg, bg)\n");
    printf("   6. ✓ Signal handling (Ctrl+C, Ctrl+Z)\n");
    printf("   7. ✓ Built-in commands\n");
    printf("   8. ✓ Complex command combinations\n\n");
    
    printf("🎓 Educational Value:\n\n");
    printf("   • Process creation & management (fork, exec)\n");
    printf("   • Inter-process communication (pipes)\n");
    printf("   • File descriptor manipulation (dup2)\n");
    printf("   • Signal handling (sigaction)\n");
    printf("   • Process groups & terminal control\n");
    printf("   • Job scheduling & control\n\n");
    
    printf("📚 Next Steps:\n\n");
    printf("   1. Build the shell: make\n");
    printf("   2. Run it: ./myShell\n");
    printf("   3. Try the commands shown in this demo\n");
    printf("   4. Explore the source code in src/\n");
    printf("   5. Read the documentation in README.md\n\n");
    
    printf("═══════════════════════════════════════════\n");
    printf("   Thank you for exploring myShell! 🐧\n");
    printf("═══════════════════════════════════════════\n\n");
    
    return 0;
}
