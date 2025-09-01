#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    // Disable buffering on stdout
    setvbuf(stdout, NULL, _IONBF, 0);
    
    // Try different output methods
    printf("1. Output using printf\n");
    fprintf(stdout, "2. Output using fprintf to stdout\n");
    write(STDOUT_FILENO, "3. Output using write to STDOUT_FILENO\n", 38);
    
    // Create diagnostic log
    int fd = open("/tmp/ms_diag.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd != -1) {
        const char *msg = "Log entry from test program\n";
        write(fd, msg, strlen(msg));
        close(fd);
    }
    
    // Flush everything
    fflush(stdout);
    
    return 0;
}
