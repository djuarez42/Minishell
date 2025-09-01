#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// Mock implementation of handle_redirections_out based on the source code
void handle_redirections_out(const char *filename, int *error) {
    int fd;

    printf("handle_redirections_out: Opening file '%s'\n", filename);
    
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open (redirect out)");
        *error = 1;
        return;
    }

    printf("Successfully opened '%s' as fd %d\n", filename, fd);

    // Get the original stdout target
    char orig_stdout_path[256] = "?";
    char path_buffer[64];
    snprintf(path_buffer, sizeof(path_buffer), "/proc/self/fd/%d", STDOUT_FILENO);
    ssize_t link_size = readlink(path_buffer, orig_stdout_path, sizeof(orig_stdout_path) - 1);
    if (link_size > 0) orig_stdout_path[link_size] = '\0';
    
    printf("Original stdout (fd %d) points to: %s\n", STDOUT_FILENO, orig_stdout_path);

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2 (redirect out)");
        close(fd);
        *error = 1;
        return;
    }

    printf("Redirected stdout to fd %d\n", fd); // This should go to the file

    // Test write to the redirected stdout
    write(STDOUT_FILENO, "REDIRECTED STDOUT TEST\n", 23);
    fflush(stdout);

    // Get the new stdout target
    char new_stdout_path[256] = "?";
    snprintf(path_buffer, sizeof(path_buffer), "/proc/self/fd/%d", STDOUT_FILENO);
    link_size = readlink(path_buffer, new_stdout_path, sizeof(new_stdout_path) - 1);
    if (link_size > 0) new_stdout_path[link_size] = '\0';
    
    int diag_fd = open("/tmp/redir_isolated_test.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (diag_fd != -1) {
        char buffer[512];
        int len = snprintf(buffer, sizeof(buffer), 
                "New stdout (fd %d) points to: %s\n", STDOUT_FILENO, new_stdout_path);
        write(diag_fd, buffer, len);
        close(diag_fd);
    }

    close(fd);
}

int main() {
    int error = 0;
    const char *test_file = "isolated_redir_test.txt";
    
    // Save original stdout
    int save_stdout = dup(STDOUT_FILENO);
    if (save_stdout == -1) {
        perror("Failed to save stdout");
        return 1;
    }
    
    // Call the redirection function
    handle_redirections_out(test_file, &error);
    
    // This should go to the redirected file
    printf("If you see this in the file, redirection worked!\n");
    fflush(stdout);
    
    // Restore stdout
    if (dup2(save_stdout, STDOUT_FILENO) == -1) {
        perror("Failed to restore stdout");
        close(save_stdout);
        return 1;
    }
    close(save_stdout);
    
    // This should go to the terminal
    printf("Redirection test complete, error = %d\n", error);
    
    // Check the output file
    int fd = open(test_file, O_RDONLY);
    if (fd != -1) {
        char buf[1024];
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("File contents (%zd bytes):\n%s\n", n, buf);
        } else {
            printf("File is empty or error reading: %s\n", strerror(errno));
        }
        close(fd);
    } else {
        printf("Could not open output file: %s\n", strerror(errno));
    }
    
    return 0;
}
