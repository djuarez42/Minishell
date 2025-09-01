#ifndef REDIRECTION_LOGGER_H
#define REDIRECTION_LOGGER_H

/**
 * Enhanced diagnostic logging function for redirection debugging
 * This logs various details about file descriptors and redirection state
 * to help identify issues with stdout capture.
 */
void log_redirection_state(const char *where, int target_fd, const char *filename);

#endif /* REDIRECTION_LOGGER_H */
