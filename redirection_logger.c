#include "minishell.h"
#include "executor.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>  // For PATH_MAX

/**
 * Enhanced diagnostic logging function for redirection debugging
 * This logs various details about file descriptors and redirection state
 * to help identify issues with stdout capture.
 */
void log_redirection_state(const char *where, int target_fd, const char *filename) {
    // Open diagnostic log
    int log_fd = open("/tmp/redir_trace.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) return;
    
    // Buffer for building log messages
    char buffer[1024];
    int len;
    
    // Log basic info
    len = snprintf(buffer, sizeof(buffer), 
            "\n=== [%s] PID=%d ===\n", where, getpid());
    write(log_fd, buffer, len);
    
    // Log target file descriptor info
    len = snprintf(buffer, sizeof(buffer), 
            "Target FD: %d, Filename: %s\n", target_fd, filename ? filename : "N/A");
    write(log_fd, buffer, len);
    
    // Check if FD is valid
    int fd_flags = fcntl(target_fd, F_GETFL);
    if (fd_flags == -1) {
        len = snprintf(buffer, sizeof(buffer), 
                "FD %d is invalid or closed: %s\n", target_fd, strerror(errno));
    } else {
        len = snprintf(buffer, sizeof(buffer), 
                "FD %d flags: 0x%x, isatty: %d\n", 
                target_fd, fd_flags, isatty(target_fd));
    }
    write(log_fd, buffer, len);
    
    // Get file descriptor paths
    char fd0_path[256] = "?", fd1_path[256] = "?", fd2_path[256] = "?";
    char path_buffer[64];
    
    for (int i = 0; i <= 2; i++) {
        snprintf(path_buffer, sizeof(path_buffer), "/proc/self/fd/%d", i);
        ssize_t link_size;
        
        if (i == 0) {
            link_size = readlink(path_buffer, fd0_path, sizeof(fd0_path) - 1);
            if (link_size > 0) fd0_path[link_size] = '\0';
        } else if (i == 1) {
            link_size = readlink(path_buffer, fd1_path, sizeof(fd1_path) - 1);
            if (link_size > 0) fd1_path[link_size] = '\0';
        } else {
            link_size = readlink(path_buffer, fd2_path, sizeof(fd2_path) - 1);
            if (link_size > 0) fd2_path[link_size] = '\0';
        }
    }
    
    len = snprintf(buffer, sizeof(buffer), 
            "FD targets - stdin: %s, stdout: %s, stderr: %s\n", 
            fd0_path, fd1_path, fd2_path);
    write(log_fd, buffer, len);
    
    // Log current working directory
    char cwd[PATH_MAX] = "?";
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "unknown");
    }
    len = snprintf(buffer, sizeof(buffer), "CWD: %s\n", cwd);
    write(log_fd, buffer, len);
    
    // Test write to stdout
    write(STDOUT_FILENO, "---STDOUT TEST---\n", 18);
    len = snprintf(buffer, sizeof(buffer), 
            "Wrote test to stdout (fd %d)\n", STDOUT_FILENO);
    write(log_fd, buffer, len);
    
    // Log end marker
    len = snprintf(buffer, sizeof(buffer), "=== End [%s] ===\n", where);
    write(log_fd, buffer, len);
    
    close(log_fd);
}
