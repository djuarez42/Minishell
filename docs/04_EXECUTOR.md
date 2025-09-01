# Executor Component

## Overview

The executor component is responsible for running commands after they have been lexed, parsed, and expanded. It handles command execution, redirections, pipes, and signals. The executor is the final step in the command processing pipeline, taking the parsed command tree and turning it into actual actions.

## Example: Processing `echo $'USER'`

### Input Command

```
echo $'USER'
```

After parsing and expansion, this is represented as:
- Command: `echo`
- Arguments: `USER` (after ANSI-C quote expansion of `$'USER'`)

### Execution Flow

1. The command tree with `echo` and its arguments is passed to the executor
2. The executor checks if `echo` is a builtin
3. Since `echo` is a builtin, it calls the builtin function directly
4. The builtin `echo` function outputs the argument `USER` to stdout
5. The exit status of the command is stored

### Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `execute_command()` | Main entry point for execution | `execute_command(cmd_node, envp, state)` |
| `execute_simple_command()` | Executes a single command | `execute_simple_command(simple_cmd, envp, state)` |
| `is_builtin()` | Checks if a command is a builtin | `is_builtin = is_builtin("echo")` |
| `execute_builtin()` | Executes a builtin command | `status = execute_builtin("echo", args, envp, state)` |
| `execute_external()` | Executes an external command | `execute_external(cmd, args, envp)` |
| `setup_redirections()` | Sets up input/output redirections | `setup_redirections(cmd->redirects)` |
| `restore_redirections()` | Restores original file descriptors | `restore_redirections(saved_fds)` |
| `handle_pipes()` | Sets up pipes between commands | `handle_pipes(pipe_fd, is_last)` |

### Variables and Data Structures

```c
typedef struct s_simple_cmd {
    char *command;           // Command name
    char **arguments;        // Command arguments
    t_redirect *redirects;   // Input/output redirections
} t_simple_cmd;

typedef struct s_cmd_node {
    enum {
        CMD_SIMPLE,
        CMD_PIPE,
        /* other command types */
    } type;
    union {
        t_simple_cmd simple;
        struct {
            struct s_cmd_node *left;
            struct s_cmd_node *right;
        } pipe;
        /* other command variants */
    } content;
} t_cmd_node;

typedef struct s_redirect {
    enum {
        REDIR_IN,            // <
        REDIR_OUT,           // >
        REDIR_APPEND,        // >>
        REDIR_HEREDOC        // <<
    } type;
    char *word;              // Filename or delimiter
    struct s_redirect *next; // Next redirection
} t_redirect;
```

### Step-by-Step Execution

For the command `echo $'USER'` (with `$'USER'` already expanded to `USER`):

1. **Initial Processing**:
   - `execute_command()` is called with the parsed command tree
   - The tree contains a simple command node with command "echo" and argument "USER"

2. **Command Type Determination**:
   - The executor checks if this is a simple command, pipe, or other structure
   - For `echo USER`, it's a simple command

3. **Redirection Setup**:
   - `setup_redirections()` is called to handle any redirections
   - For `echo USER`, there are no redirections, so standard input/output are used

4. **Builtin Check**:
   - `is_builtin()` is called to check if "echo" is a builtin command
   - "echo" is identified as a builtin

5. **Builtin Execution**:
   - `execute_builtin()` is called with "echo" and arguments ["USER"]
   - The builtin echo implementation prints "USER" to standard output

6. **Exit Status Management**:
   - The exit status (0 for success) is stored in the execution state
   - This value can be accessed using `$?` in subsequent commands

7. **Cleanup**:
   - If any redirections were set up, they are restored

### Source Code Example

```c
int execute_command(t_cmd_node *cmd_node, char **envp, t_exec_state *state)
{
    int status = 0;
    
    if (!cmd_node)
        return 0;
        
    if (cmd_node->type == CMD_SIMPLE)
    {
        status = execute_simple_command(&cmd_node->content.simple, envp, state);
    }
    else if (cmd_node->type == CMD_PIPE)
    {
        status = execute_pipe_command(cmd_node, envp, state);
    }
    // Handle other command types...
    
    state->last_status = status;
    return status;
}

int execute_simple_command(t_simple_cmd *cmd, char **envp, t_exec_state *state)
{
    int status = 0;
    int saved_fds[2];
    
    // Save current stdin/stdout
    saved_fds[0] = dup(STDIN_FILENO);
    saved_fds[1] = dup(STDOUT_FILENO);
    
    // Set up redirections
    if (cmd->redirects && setup_redirections(cmd->redirects) != 0)
    {
        restore_redirections(saved_fds);
        return 1;  // Error in redirection
    }
    
    // Execute the command
    if (is_builtin(cmd->command))
    {
        status = execute_builtin(cmd->command, cmd->arguments, envp, state);
    }
    else
    {
        status = execute_external(cmd->command, cmd->arguments, envp);
    }
    
    // Restore original stdin/stdout
    restore_redirections(saved_fds);
    
    return status;
}

int execute_builtin(const char *cmd, char **args, char **envp, t_exec_state *state)
{
    if (ft_strcmp(cmd, "echo") == 0)
        return builtin_echo(args);
    else if (ft_strcmp(cmd, "cd") == 0)
        return builtin_cd(args, envp);
    // Other builtins...
    
    return 1;  // Command not found
}

int builtin_echo(char **args)
{
    int i = 1;
    int print_newline = 1;
    
    // Check for -n option
    if (args[i] && ft_strcmp(args[i], "-n") == 0)
    {
        print_newline = 0;
        i++;
    }
    
    // Print arguments
    while (args[i])
    {
        ft_putstr_fd(args[i], STDOUT_FILENO);
        if (args[i + 1])
            ft_putchar_fd(' ', STDOUT_FILENO);
        i++;
    }
    
    // Print newline if needed
    if (print_newline)
        ft_putchar_fd('\n', STDOUT_FILENO);
        
    return 0;  // Success
}
```

### Echo Builtin Implementation

The `echo` builtin follows these rules:
1. Prints all arguments separated by spaces
2. If `-n` option is provided, doesn't print a trailing newline
3. Returns 0 (success) exit status

For `echo $'USER'` (after expansion to `echo USER`):
1. `builtin_echo()` is called with args = ["echo", "USER"]
2. No `-n` option is detected
3. "USER" is printed to standard output
4. A newline is added
5. The function returns 0 (success)

### Process Creation for External Commands

For non-builtin commands, the executor:
1. Forks a new process
2. Child process replaces itself with the external command using `execve()`
3. Parent process waits for child to complete
4. Parent process records exit status

For our example `echo` (if it were not a builtin):
```c
int execute_external(const char *cmd, char **args, char **envp)
{
    pid_t pid;
    int status;
    char *cmd_path;
    
    // Find command in PATH
    cmd_path = find_command_path(cmd, envp);
    if (!cmd_path)
        return 127;  // Command not found
    
    pid = fork();
    if (pid == 0)
    {
        // Child process
        execve(cmd_path, args, envp);
        // If execve returns, there was an error
        exit(127);
    }
    else if (pid > 0)
    {
        // Parent process
        waitpid(pid, &status, 0);
        free(cmd_path);
        
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return 1;
    }
    
    free(cmd_path);
    return 1;  // Fork error
}
```
