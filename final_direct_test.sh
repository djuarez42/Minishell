#!/bin/bash

echo "=== Final Direct Test with Tracing ==="
cd /home/ekakhmad/Minishell

# Clean up any old files
rm -f direct_test_output.txt

# Create a diagnostic echo-redirection standalone program
cat > echo_redirect_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    printf("Echo-Redirect Test\n");
    
    // Create a file directly - sanity check
    int direct_fd = open("direct_write.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (direct_fd != -1) {
        write(direct_fd, "Direct write test\n", 18);
        close(direct_fd);
        printf("Direct write successful\n");
    } else {
        perror("direct write");
    }
    
    // Execute the shell with a redirect command
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        char *args[] = {"./minishell", NULL};
        
        // Set up pipe to send commands to minishell
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(1);
        }
        
        // Set stdin to read from our pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        
        // Close unused pipe ends
        close(pipefd[0]);
        close(pipefd[1]);
        
        // Execute minishell
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    } else {
        // Parent process
        // Open a pipe to send commands to minishell
        FILE *pipe = popen("./minishell", "w");
        if (pipe) {
            fprintf(pipe, "echo FINAL_TEST > direct_test_output.txt\n");
            fprintf(pipe, "exit\n");
            pclose(pipe);
        } else {
            perror("popen");
        }
        
        // Wait for minishell to finish
        int status;
        waitpid(pid, &status, 0);
        
        // Check if file was created
        printf("Checking for output file...\n");
        if (access("direct_test_output.txt", F_OK) == 0) {
            printf("File exists!\n");
            // Display contents
            FILE *f = fopen("direct_test_output.txt", "r");
            if (f) {
                char buf[100];
                size_t n = fread(buf, 1, sizeof(buf)-1, f);
                buf[n] = '\0';
                printf("File contents: %s\n", buf);
                fclose(f);
            } else {
                perror("fopen");
            }
        } else {
            printf("File does not exist!\n");
        }
    }
    
    return 0;
}
EOF

# Compile and run
gcc -o echo_redirect_test echo_redirect_test.c
./echo_redirect_test

# Print environment info
echo
echo "=== Environment Info ==="
echo "Current directory: $(pwd)"
echo "Shell: $SHELL"
echo "Terminal: $TERM"
ls -la direct_*.txt
