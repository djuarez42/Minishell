#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

void log_fd_state(const char *location, int fd) {
    int log_fd = open("/tmp/fd_trace.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) return;
    
    char buffer[1024];
    int len;
    
    // Log basic info
    len = snprintf(buffer, sizeof(buffer), 
            "\n=== [%s] PID=%d ===\n", location, getpid());
    write(log_fd, buffer, len);
    
    // Check if FD is valid
    int fd_flags = fcntl(fd, F_GETFL);
    if (fd_flags == -1) {
        len = snprintf(buffer, sizeof(buffer), 
                "FD %d is invalid or closed: %s\n", fd, strerror(errno));
    } else {
        len = snprintf(buffer, sizeof(buffer), 
                "FD %d flags: 0x%x, isatty: %d\n", 
                fd, fd_flags, isatty(fd));
    }
    write(log_fd, buffer, len);
    
    // Get file descriptor path
    char fd_path[256] = "?";
    char path_buffer[64];
    
    snprintf(path_buffer, sizeof(path_buffer), "/proc/self/fd/%d", fd);
    ssize_t link_size = readlink(path_buffer, fd_path, sizeof(fd_path) - 1);
    if (link_size > 0) fd_path[link_size] = '\0';
    
    len = snprintf(buffer, sizeof(buffer), "FD %d target: %s\n", fd, fd_path);
    write(log_fd, buffer, len);
    
    close(log_fd);
}

int run_minishell_with_redirection() {
    char cmd[] = "echo testing > minishell_test_output.txt";
    
    // Create pipes for communication
    int pipes[2];
    if (pipe(pipes) == -1) {
        perror("pipe");
        return 1;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipes[0]);
        close(pipes[1]);
        return 1;
    }
    
    if (pid == 0) {
        // Child process
        close(pipes[0]);  // Close read end
        
        // Redirect stdin to our pipe
        if (dup2(pipes[1], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(pipes[1]);
        
        // Create detailed log directory
        system("mkdir -p /tmp/minishell_test");
        
        // Log stdin before exec
        log_fd_state("minishell-child-before-exec", STDIN_FILENO);
        
        // Execute minishell
        execl("./minishell", "minishell", NULL);
        
        // If we get here, exec failed
        perror("execl");
        exit(1);
    } else {
        // Parent process
        close(pipes[1]);  // Close write end
        
        // Write command to minishell
        write(pipes[0], cmd, strlen(cmd));
        write(pipes[0], "\n", 1);
        close(pipes[0]);
        
        // Wait for child to complete
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Minishell exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Minishell terminated by signal: %d\n", WTERMSIG(status));
        }
        
        // Check if output file exists
        if (access("minishell_test_output.txt", F_OK) != -1) {
            printf("Output file exists\n");
            
            // Display file contents
            int fd = open("minishell_test_output.txt", O_RDONLY);
            if (fd != -1) {
                char buf[1024];
                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    printf("File contents: %s\n", buf);
                } else {
                    printf("File is empty\n");
                }
                close(fd);
            } else {
                printf("Could not open output file: %s\n", strerror(errno));
            }
        } else {
            printf("Output file does not exist\n");
        }
        
        return 0;
    }
}

int main() {
    printf("=== Starting Enhanced Redirection Diagnostic ===\n");
    
    // Test 1: Direct file output in parent process
    printf("Test 1: Direct file output\n");
    int fd = open("diagnostic_direct_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        log_fd_state("direct-output-before", fd);
        write(fd, "Test direct output\n", 19);
        log_fd_state("direct-output-after", fd);
        close(fd);
        printf("Direct output test complete\n");
    } else {
        printf("Failed to open diagnostic_direct_output.txt: %s\n", strerror(errno));
    }
    
    // Test 2: Stdout redirection in parent process
    printf("\nTest 2: Stdout redirection\n");
    log_fd_state("stdout-before-redirect", STDOUT_FILENO);
    
    // Save original stdout
    int save_stdout = dup(STDOUT_FILENO);
    if (save_stdout == -1) {
        printf("Failed to save stdout: %s\n", strerror(errno));
    } else {
        // Open output file
        fd = open("diagnostic_stdout_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd != -1) {
            // Redirect stdout
            if (dup2(fd, STDOUT_FILENO) != -1) {
                log_fd_state("stdout-after-redirect", STDOUT_FILENO);
                
                // This should go to the file
                printf("This is redirected stdout\n");
                write(STDOUT_FILENO, "Direct write to redirected stdout\n", 33);
                fflush(stdout);
                
                // Restore stdout
                dup2(save_stdout, STDOUT_FILENO);
                log_fd_state("stdout-after-restore", STDOUT_FILENO);
                
                printf("Stdout redirection test complete\n");
            } else {
                printf("Failed to redirect stdout: %s\n", strerror(errno));
            }
            close(fd);
        } else {
            printf("Failed to open diagnostic_stdout_output.txt: %s\n", strerror(errno));
        }
        close(save_stdout);
    }
    
    // Test 3: Run minishell with redirection
    printf("\nTest 3: Minishell redirection\n");
    run_minishell_with_redirection();
    
    printf("\n=== Enhanced Redirection Diagnostic Complete ===\n");
    return 0;
}
