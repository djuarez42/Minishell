#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

// Minimal test program to diagnose redirection issues in minishell
int main() {
    // Create a file with diagnostic info
    int diag_fd = open("/tmp/redirect_diag.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (diag_fd != -1) {
        char buf[256];
        int len = snprintf(buf, sizeof(buf), "===== New test at PID %d =====\n", getpid());
        write(diag_fd, buf, len);
        
        // Log the initial file descriptors
        len = snprintf(buf, sizeof(buf), "Initial stdin=%d, stdout=%d, stderr=%d\n", 
                      STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
        write(diag_fd, buf, len);
        
        // Check if stdout is a terminal
        int is_term = isatty(STDOUT_FILENO);
        len = snprintf(buf, sizeof(buf), "stdout is %s terminal\n", 
                      is_term ? "a" : "NOT a");
        write(diag_fd, buf, len);
        
        // Get stdout flags
        int flags = fcntl(STDOUT_FILENO, F_GETFL);
        len = snprintf(buf, sizeof(buf), "stdout flags: 0x%x\n", flags);
        write(diag_fd, buf, len);
        
        close(diag_fd);
    }
    
    // Basic output test
    printf("STDOUT: Basic printf test\n");
    fflush(stdout);
    
    fprintf(stderr, "STDERR: Basic stderr test\n");
    
    write(STDOUT_FILENO, "STDOUT: Basic write test\n", 25);
    
    // Test output redirection
    const char *test_file = "redirection_test_output.txt";
    printf("STDOUT: Attempting to write to file: %s\n", test_file);
    fflush(stdout);
    
    // Create a file with direct file operations (should work regardless of redirection)
    FILE *direct_file = fopen("direct_file_output.txt", "w");
    if (direct_file) {
        fprintf(direct_file, "This was written with direct FILE* operations\n");
        fclose(direct_file);
    }
    
    return 0;
}
