#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "include/lexer.h"

// We use lexer.h for the token type constants
int main() {
    printf("=== Minishell Direct Redirection Test ===\n");
    
    // This simulates creating a file with the > operator
    printf("Testing TOKEN_REDIRECT_OUT (>) operator...\n");
    
    // Open file for output redirection
    int fd = open("direct_redirect_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }
    
    printf("File opened with fd=%d\n", fd);
    
    // Save original stdout
    int original_stdout = dup(STDOUT_FILENO);
    if (original_stdout == -1) {
        perror("Failed to duplicate stdout");
        close(fd);
        return 1;
    }
    
    // Redirect stdout to file
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("Failed to redirect stdout");
        close(fd);
        close(original_stdout);
        return 1;
    }
    
    // This should go to the file
    printf("This text should be in the redirected file\n");
    printf("Token type constants from lexer.h:\n");
    printf("  TOKEN_REDIRECT_IN = %d\n", TOKEN_REDIRECT_IN);
    printf("  TOKEN_REDIRECT_OUT = %d\n", TOKEN_REDIRECT_OUT);
    printf("  TOKEN_HEREDOC = %d\n", TOKEN_HEREDOC);
    printf("  TOKEN_APPEND = %d\n", TOKEN_APPEND);
    fflush(stdout);
    
    // Restore stdout
    if (dup2(original_stdout, STDOUT_FILENO) == -1) {
        perror("Failed to restore stdout");
        close(fd);
        close(original_stdout);
        return 1;
    }
    
    close(fd);
    close(original_stdout);
    
    // Print results
    printf("\nRedirection test completed. Checking results...\n");
    
    // Read back the file to confirm contents
    fd = open("direct_redirect_test.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file for reading");
        return 1;
    }
    
    char buffer[512];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("\nFile contents:\n%s\n", buffer);
    } else {
        printf("Failed to read file or file is empty\n");
    }
    
    close(fd);
    
    printf("\n=== Test Complete ===\n");
    return 0;
}
