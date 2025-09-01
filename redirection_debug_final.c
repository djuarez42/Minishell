#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

// Enhanced diagnostic program to isolate and fix redirection issues in minishell
int main() {
    // Initialize diagnostic log
    int diag_fd = open("/tmp/stdout_diag.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (diag_fd != -1) {
        dprintf(diag_fd, "=== Starting diagnostic run (PID: %d) ===\n", getpid());
        
        // Log initial file descriptor information
        dprintf(diag_fd, "Initial stdout fd: %d (isatty: %d)\n", 
                STDOUT_FILENO, isatty(STDOUT_FILENO));
        
        // Log file descriptor flags
        int flags = fcntl(STDOUT_FILENO, F_GETFL);
        dprintf(diag_fd, "Stdout flags: 0x%x\n", flags);
        
        // Test stdout functionality
        dprintf(diag_fd, "About to write to stdout...\n");
        printf("STDOUT: Direct printf output\n");
        fflush(stdout);
        dprintf(diag_fd, "Printf to stdout completed\n");
        
        // Test direct file operations
        dprintf(diag_fd, "Creating direct output file...\n");
        FILE *direct_file = fopen("/tmp/direct_output.txt", "w");
        if (direct_file) {
            fprintf(direct_file, "DIRECT: Output using direct file operations\n");
            fclose(direct_file);
            dprintf(diag_fd, "Direct file operations completed\n");
        } else {
            dprintf(diag_fd, "ERROR: Could not create direct file: %s\n", strerror(errno));
        }
        
        // Test redirection with dup2
        dprintf(diag_fd, "Testing manual redirection...\n");
        int original_stdout = dup(STDOUT_FILENO);
        int redirect_fd = open("/tmp/redirected_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
        if (redirect_fd != -1) {
            dprintf(diag_fd, "Opened redirect file as fd: %d\n", redirect_fd);
            
            // Redirect stdout
            dprintf(diag_fd, "About to redirect stdout with dup2(%d, %d)...\n", 
                    redirect_fd, STDOUT_FILENO);
            if (dup2(redirect_fd, STDOUT_FILENO) != -1) {
                dprintf(diag_fd, "Redirection successful\n");
                
                // Test redirected output
                printf("REDIRECTED: This should go to the redirected file\n");
                fflush(stdout);
                write(STDOUT_FILENO, "REDIRECTED: Using write to STDOUT_FILENO\n", 40);
                
                // Restore original stdout
                dprintf(diag_fd, "Restoring original stdout...\n");
                dup2(original_stdout, STDOUT_FILENO);
                dprintf(diag_fd, "Original stdout restored\n");
            } else {
                dprintf(diag_fd, "ERROR: dup2 failed: %s\n", strerror(errno));
            }
            
            close(redirect_fd);
        } else {
            dprintf(diag_fd, "ERROR: Could not open redirect file: %s\n", strerror(errno));
        }
        
        // Clean up
        close(original_stdout);
        
        // Try in-program pipe redirection
        dprintf(diag_fd, "Testing pipe redirection...\n");
        int pipe_fds[2];
        if (pipe(pipe_fds) != -1) {
            dprintf(diag_fd, "Created pipe: read_fd=%d, write_fd=%d\n", pipe_fds[0], pipe_fds[1]);
            
            // Redirect stdout to pipe write end
            original_stdout = dup(STDOUT_FILENO);
            if (dup2(pipe_fds[1], STDOUT_FILENO) != -1) {
                dprintf(diag_fd, "Redirected stdout to pipe\n");
                
                // Write to stdout (which is now the pipe)
                printf("PIPE: This should go through the pipe\n");
                fflush(stdout);
                
                // Restore stdout
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                
                // Read from pipe
                char buffer[1024];
                close(pipe_fds[1]);  // Close write end
                int bytes_read = read(pipe_fds[0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    dprintf(diag_fd, "Read from pipe: '%s'\n", buffer);
                } else {
                    dprintf(diag_fd, "ERROR: No data read from pipe: %s\n", strerror(errno));
                }
                close(pipe_fds[0]);
            } else {
                dprintf(diag_fd, "ERROR: Failed to redirect stdout to pipe: %s\n", strerror(errno));
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            }
        } else {
            dprintf(diag_fd, "ERROR: Failed to create pipe: %s\n", strerror(errno));
        }
        
        dprintf(diag_fd, "=== Diagnostic run completed ===\n");
        close(diag_fd);
    }
    
    // Final output to stdout - should be visible in normal stdout
    printf("FINAL: If you see this, stdout is working properly.\n");
    
    return 0;
}
