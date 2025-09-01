#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    
    // Print to stdout
    printf("Hello from test program\n");
    fflush(stdout);
    
    // Also try write
    write(STDOUT_FILENO, "Message using write()\n", 21);
    
    return 0;
}
