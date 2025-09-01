#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    FILE *debug_log = fopen("/tmp/redirect_debug.log", "a");
    if (debug_log) {
        fprintf(debug_log, "=== New test run ===\n");
        fclose(debug_log);
    }

    // Test stdout before any changes
    printf("Initial stdout test\n");
    fflush(stdout);
    
    // Create a file for redirection
    int fd = open("output_test_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open output file");
        return 1;
    }
    
    // Log the file descriptor
    debug_log = fopen("/tmp/redirect_debug.log", "a");
    if (debug_log) {
        fprintf(debug_log, "Opened file descriptor: %d\n", fd);
        fclose(debug_log);
    }
    
    // Save original stdout
    int original_stdout = dup(STDOUT_FILENO);
    
    // Redirect stdout to the file
    debug_log = fopen("/tmp/redirect_debug.log", "a");
    if (debug_log) {
        fprintf(debug_log, "About to redirect stdout using dup2(%d, %d)\n", fd, STDOUT_FILENO);
        fclose(debug_log);
    }
    
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("Failed to redirect stdout");
        close(fd);
        return 1;
    }
    
    // Test output after redirection
    printf("This should go to the file\n");
    fprintf(stdout, "This fprintf should also go to the file\n");
    write(STDOUT_FILENO, "Write to STDOUT_FILENO should go to the file\n", 45);
    fflush(stdout);
    
    // Restore original stdout
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);
    close(fd);
    
    // This should go to the console again
    printf("Back to original stdout\n");
    
    return 0;
}
