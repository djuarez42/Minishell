#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main() {
    printf("=== Redirection Test with Shell Tracing ===\n");
    
    // Run a shell session with trace enabled
    printf("Running 'echo hello > traced_output.txt' with shell trace\n");
    
    // Create a detailed diagnostic script
    int script_fd = open("trace_test.sh", O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (script_fd != -1) {
        const char *script = 
            "#!/bin/bash\n"
            "set -x\n"  // Enable tracing
            "cd /home/ekakhmad/Minishell\n"
            "./minishell << 'EOF'\n"
            "echo DIRECT_TEST > direct_stdout_test.txt\n"
            "EOF\n"
            "echo \"Exit status: $?\"\n"
            "ls -la direct_stdout_test.txt\n"
            "cat direct_stdout_test.txt\n";
        
        write(script_fd, script, strlen(script));
        close(script_fd);
        
        // Run the script
        system("./trace_test.sh > trace_output.txt 2>&1");
        
        // Display the results
        printf("Trace complete. Results:\n");
        system("cat trace_output.txt");
    } else {
        printf("Failed to create trace script\n");
    }
    
    return 0;
}
