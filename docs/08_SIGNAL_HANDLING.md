# Signal Handling Component

## Overview

The signal handling component manages how the shell responds to various signals, primarily SIGINT (Ctrl+C), SIGQUIT (Ctrl+\\), and SIGTERM. Proper signal handling is crucial for a shell to behave correctly in different contexts, such as when running in interactive mode versus executing commands.

## Signal Types and Default Behaviors

| Signal | Key Combination | Default Action | Shell Behavior |
|--------|----------------|----------------|---------------|
| SIGINT | Ctrl+C | Terminate | In interactive mode: Display a new prompt<br>In command execution: Terminate command |
| SIGQUIT | Ctrl+\\ | Terminate with core dump | In interactive mode: Ignore<br>In command execution: Pass to command |
| SIGTSTP | Ctrl+Z | Stop process | In interactive mode: Ignore<br>In command execution: Pass to command |
| SIGTERM | `kill` command | Terminate | Terminate the shell |

## Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `setup_signals()` | Sets up signal handlers for the shell | `setup_signals(INTERACTIVE_MODE)` |
| `reset_signals()` | Restores default signal handling | `reset_signals()` |
| `handle_sigint()` | Handler for SIGINT signal | Called automatically on SIGINT |
| `handle_sigquit()` | Handler for SIGQUIT signal | Called automatically on SIGQUIT |
| `ignore_signals()` | Ignores specified signals | `ignore_signals()` |

## Implementation

### Signal Mode Enumeration

```c
typedef enum e_signal_mode {
    INTERACTIVE_MODE,  // Regular shell prompt
    EXECUTING_MODE,    // Running a command
    HEREDOC_MODE       // Reading a heredoc
} t_signal_mode;
```

### Signal Handler Setup

```c
void setup_signals(t_signal_mode mode)
{
    if (mode == INTERACTIVE_MODE)
    {
        // Interactive mode (shell prompt)
        signal(SIGINT, handle_sigint_interactive);
        signal(SIGQUIT, SIG_IGN);  // Ignore Ctrl+\
    }
    else if (mode == EXECUTING_MODE)
    {
        // Executing a command
        signal(SIGINT, handle_sigint_executing);
        signal(SIGQUIT, handle_sigquit_executing);
    }
    else if (mode == HEREDOC_MODE)
    {
        // Reading a heredoc
        signal(SIGINT, handle_sigint_heredoc);
        signal(SIGQUIT, SIG_IGN);  // Ignore Ctrl+\
    }
}
```

### Signal Handlers

```c
void handle_sigint_interactive(int sig)
{
    (void)sig;  // Unused parameter
    
    // Print a newline and redisplay the prompt
    ft_putchar_fd('\n', STDERR_FILENO);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    
    // Set exit status to 1
    g_signal_status = 1;
}

void handle_sigint_executing(int sig)
{
    (void)sig;  // Unused parameter
    
    // Simply print a newline, the child process will be terminated
    ft_putchar_fd('\n', STDERR_FILENO);
    
    // Set exit status to 130 (128 + SIGINT)
    g_signal_status = 130;
}

void handle_sigquit_executing(int sig)
{
    (void)sig;  // Unused parameter
    
    // Print "Quit" message
    ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
    
    // Set exit status to 131 (128 + SIGQUIT)
    g_signal_status = 131;
}

void handle_sigint_heredoc(int sig)
{
    (void)sig;  // Unused parameter
    
    // Print a newline and exit the heredoc
    ft_putchar_fd('\n', STDERR_FILENO);
    close(STDIN_FILENO);  // This will cause the read in the heredoc to fail
    
    // Set exit status to 1
    g_signal_status = 1;
}
```

### Global Variable for Signal Status

```c
// Global variable to track signal status
volatile sig_atomic_t g_signal_status = 0;
```

### Resetting Signals to Default

```c
void reset_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
```

### Ignoring Signals

```c
void ignore_signals(void)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}
```

## Signal Handling in Different Contexts

### Main Shell Loop

In the main shell loop, signals are set to interactive mode:

```c
int main(int argc, char **argv, char **envp)
{
    // Initialize shell
    // ...
    
    // Set up signals for interactive mode
    setup_signals(INTERACTIVE_MODE);
    
    // Main shell loop
    while (1)
    {
        // Display prompt and read input
        input = readline("minishell> ");
        
        if (!input)
            break;  // EOF (Ctrl+D)
            
        // Process input
        // ...
        
        // Execute command
        if (cmd_tree)
        {
            // Set signals for command execution
            setup_signals(EXECUTING_MODE);
            
            // Execute command
            execute_command(cmd_tree, shell_env, &state);
            
            // Restore signals for interactive mode
            setup_signals(INTERACTIVE_MODE);
        }
        
        // Cleanup
        // ...
    }
    
    // Cleanup and exit
    // ...
    
    return 0;
}
```

### Command Execution

When executing a command, the parent shell process ignores signals, while the child process resets them to default:

```c
int execute_external(const char *cmd, char **args, char **envp)
{
    pid_t pid;
    int status;
    
    pid = fork();
    if (pid == 0)
    {
        // Child process
        reset_signals();  // Reset signals to default behavior
        
        execve(cmd, args, envp);
        exit(127);  // Command not found
    }
    else if (pid > 0)
    {
        // Parent process
        ignore_signals();  // Ignore signals during wait
        
        waitpid(pid, &status, 0);
        
        // Check if process was terminated by a signal
        if (WIFSIGNALED(status))
        {
            if (WTERMSIG(status) == SIGINT)
                g_signal_status = 130;
            else if (WTERMSIG(status) == SIGQUIT)
                g_signal_status = 131;
                
            return g_signal_status;
        }
        
        // Normal exit
        return WEXITSTATUS(status);
    }
    
    return 1;  // Fork error
}
```

### Heredoc Processing

During heredoc processing, special signal handling is needed:

```c
int handle_heredoc(const char *delimiter)
{
    int pipe_fd[2];
    char *line;
    
    if (pipe(pipe_fd) == -1)
        return 1;
    
    // Set up signals for heredoc mode
    setup_signals(HEREDOC_MODE);
    
    // Read input until delimiter is encountered
    while (1)
    {
        ft_putstr_fd("> ", STDERR_FILENO);
        line = read_line();
        
        // Check if read was interrupted by SIGINT
        if (!line && g_signal_status == 1)
        {
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            
            // Restore signals for interactive mode
            setup_signals(INTERACTIVE_MODE);
            return 1;
        }
        
        if (!line)
            break;
            
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        
        ft_putendl_fd(line, pipe_fd[1]);
        free(line);
    }
    
    // Close write end of pipe
    close(pipe_fd[1]);
    
    // Redirect stdin to read end of pipe
    dup2(pipe_fd[0], STDIN_FILENO);
    close(pipe_fd[0]);
    
    // Restore signals for interactive mode
    setup_signals(INTERACTIVE_MODE);
    
    return 0;
}
```

## Signal-Safe Functions

Care must be taken to use only async-signal-safe functions within signal handlers:

```c
void handle_sigint_interactive(int sig)
{
    (void)sig;
    
    // Write and ioctl are async-signal-safe
    write(STDERR_FILENO, "\n", 1);
    
    // These readline functions are not strictly async-signal-safe,
    // but they're commonly used in practice
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    
    g_signal_status = 1;
}
```

## Example: Signal Handling with `echo $'USER'`

For our example command `echo $'USER'`:

1. **Initial State**:
   - Shell is in interactive mode
   - SIGINT handler displays a new prompt
   - SIGQUIT is ignored

2. **Command Execution**:
   - Signal handlers switch to executing mode
   - If user presses Ctrl+C during execution:
     - The command is terminated
     - Shell displays a new prompt
     - Exit status is set to 130
   - Since `echo` is a builtin, it executes quickly and likely won't be interrupted

3. **Completion**:
   - Signal handlers switch back to interactive mode
   - Shell is ready for the next command

## Handling Terminal Attributes

Terminal attributes may need to be managed for proper signal handling:

```c
struct termios original_termios;

void save_terminal_settings(void)
{
    tcgetattr(STDIN_FILENO, &original_termios);
}

void restore_terminal_settings(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}
```

## Summary of Signal Behaviors

1. **Interactive Mode**:
   - SIGINT (Ctrl+C): Display new prompt
   - SIGQUIT (Ctrl+\\): Ignore
   - SIGTSTP (Ctrl+Z): Ignore

2. **Command Execution**:
   - SIGINT: Terminate command, display new prompt
   - SIGQUIT: Terminate command with "Quit" message
   - SIGTSTP: Pass to command

3. **Heredoc Mode**:
   - SIGINT: Cancel heredoc, return to prompt
   - SIGQUIT: Ignore
   - SIGTSTP: Ignore
