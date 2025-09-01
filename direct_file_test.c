#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    printf("Testing file creation with correct token type values...\n");
    
    // Use the correct token type values as defined in our fix
    #define REDIR_OUT 3  /* > */
    
    int fd;
    const char *filename = "direct_test_file.txt";
    const char *message = "Test content created with REDIR_OUT=3\n";
    
    // Create the file using REDIR_OUT token type
    printf("Opening file %s with O_CREAT|O_WRONLY|O_TRUNC flags...\n", filename);
    fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    printf("Writing test message to file...\n");
    write(fd, message, strlen(message));
    close(fd);
    
    printf("File operations completed. Checking file...\n");
    
    // Check if the file exists and display its content
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Failed to open %s for reading\n", filename);
        return 1;
    }
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("Read from file: %s", buffer);
    }
    
    fclose(fp);
    printf("Test completed successfully.\n");
    
    return 0;
}
