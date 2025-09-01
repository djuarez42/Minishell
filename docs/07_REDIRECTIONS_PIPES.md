# Redirection and Pipes Component

## Overview

The redirection and pipes component handles input/output redirection and the creation of pipelines between commands. This includes handling file redirections (`<`, `>`, `>>`), heredocs (`<<`), and pipes (`|`). These mechanisms allow commands to receive input from and send output to files or other commands.

## Redirection Types

| Type | Symbol | Purpose | Example |
|------|--------|---------|---------|
| Input | `<` | Redirects file contents to stdin | `cat < file.txt` |
| Output | `>` | Redirects stdout to a file (truncate) | `echo hello > file.txt` |
| Append | `>>` | Redirects stdout to a file (append) | `echo hello >> file.txt` |
| Heredoc | `<<` | Uses a delimiter to specify inline input | `cat << EOF ... EOF` |
| Pipe | `\|` | Connects stdout of one command to stdin of another | `echo hello \| grep h` |

## Example: Using Redirections and Pipes

While our main example `echo $'USER'` doesn't use redirections or pipes, let's examine how these features would be processed for commands like:

1. `echo $'USER' > output.txt`
2. `cat < input.txt | grep pattern`
3. `cat << EOF ... EOF`

### Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `setup_redirections()` | Sets up all redirections for a command | `setup_redirections(cmd->redirects)` |
| `handle_input_redirect()` | Handles `<` redirection | `handle_input_redirect(redirect->word)` |
| `handle_output_redirect()` | Handles `>` redirection | `handle_output_redirect(redirect->word, O_TRUNC)` |
| `handle_append_redirect()` | Handles `>>` redirection | `handle_output_redirect(redirect->word, O_APPEND)` |
| `handle_heredoc()` | Handles `<<` heredoc | `handle_heredoc(redirect->word)` |
| `create_pipe()` | Creates a pipe between commands | `create_pipe(pipe_fd)` |
| `setup_pipe_input()` | Sets up stdin from a pipe | `setup_pipe_input(pipe_fd)` |
| `setup_pipe_output()` | Sets up stdout to a pipe | `setup_pipe_output(pipe_fd)` |
| `restore_redirections()` | Restores original file descriptors | `restore_redirections(saved_fds)` |

### Variables and Data Structures

```c
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
```

### Redirection Implementation

```c
int setup_redirections(t_redirect *redirects)
{
    t_redirect *current = redirects;
    
    while (current)
    {
        switch (current->type)
        {
            case REDIR_IN:
                if (handle_input_redirect(current->word) != 0)
                    return 1;
                break;
            case REDIR_OUT:
                if (handle_output_redirect(current->word, O_TRUNC) != 0)
                    return 1;
                break;
            case REDIR_APPEND:
                if (handle_output_redirect(current->word, O_APPEND) != 0)
                    return 1;
                break;
            case REDIR_HEREDOC:
                if (handle_heredoc(current->word) != 0)
                    return 1;
                break;
        }
        current = current->next;
    }
    
    return 0;
}

int handle_input_redirect(const char *filename)
{
    int fd;
    
    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(filename, STDERR_FILENO);
        ft_putstr_fd(": ", STDERR_FILENO);
        ft_putendl_fd(strerror(errno), STDERR_FILENO);
        return 1;
    }
    
    // Redirect stdin to the file
    dup2(fd, STDIN_FILENO);
    close(fd);
    
    return 0;
}

int handle_output_redirect(const char *filename, int flags)
{
    int fd;
    
    fd = open(filename, O_WRONLY | O_CREAT | flags, 0644);
    if (fd == -1)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(filename, STDERR_FILENO);
        ft_putstr_fd(": ", STDERR_FILENO);
        ft_putendl_fd(strerror(errno), STDERR_FILENO);
        return 1;
    }
    
    // Redirect stdout to the file
    dup2(fd, STDOUT_FILENO);
    close(fd);
    
    return 0;
}

int handle_heredoc(const char *delimiter)
{
    int pipe_fd[2];
    char *line;
    
    if (pipe(pipe_fd) == -1)
        return 1;
    
    // Read input until delimiter is encountered
    while (1)
    {
        ft_putstr_fd("> ", STDERR_FILENO);
        line = read_line();
        
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
    
    return 0;
}
```

### Pipe Implementation

```c
int execute_pipe_command(t_cmd_node *cmd_node, char **envp, t_exec_state *state)
{
    int pipe_fd[2];
    pid_t pid1, pid2;
    int status1, status2;
    
    if (pipe(pipe_fd) == -1)
        return 1;
    
    // Fork for left command
    pid1 = fork();
    if (pid1 == 0)
    {
        // Child process for left command
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        
        execute_command(cmd_node->content.pipe.left, envp, state);
        exit(state->last_status);
    }
    
    // Fork for right command
    pid2 = fork();
    if (pid2 == 0)
    {
        // Child process for right command
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        
        execute_command(cmd_node->content.pipe.right, envp, state);
        exit(state->last_status);
    }
    
    // Parent process
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    
    // Wait for both processes
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    
    // Return status of right command
    if (WIFEXITED(status2))
        return WEXITSTATUS(status2);
    else
        return 1;
}
```

### Step-by-Step Example: `echo $'USER' > output.txt`

1. **Parsing**:
   - The parser identifies `echo` as the command
   - `$'USER'` is recognized as an argument
   - `> output.txt` is recognized as an output redirection

2. **Command Tree Creation**:
   - A simple command node is created
   - Command: `echo`
   - Argument: `$'USER'` (will be expanded later)
   - Redirection: `output.txt` with type `REDIR_OUT`

3. **Variable Expansion**:
   - `$'USER'` is expanded to `USER` by the expansion component

4. **Redirection Setup**:
   - `setup_redirections()` is called
   - `handle_output_redirect("output.txt", O_TRUNC)` is called
   - A file descriptor is opened for `output.txt` with O_WRONLY | O_CREAT | O_TRUNC
   - `dup2()` redirects stdout to this file descriptor

5. **Command Execution**:
   - `execute_builtin("echo", ["echo", "USER"], envp, state)` is called
   - The output "USER" is written to stdout, which is now redirected to the file
   - The command returns status 0 (success)

6. **Cleanup**:
   - Original file descriptors are restored

### Step-by-Step Example: `cat < input.txt | grep pattern`

1. **Parsing**:
   - The parser identifies a pipe structure
   - Left command: `cat < input.txt`
   - Right command: `grep pattern`

2. **Command Tree Creation**:
   - A pipe command node is created
   - Left node: simple command with `cat` and input redirection from `input.txt`
   - Right node: simple command with `grep` and argument `pattern`

3. **Pipe Creation**:
   - `pipe()` is called to create a pipe with two file descriptors

4. **Left Command Execution**:
   - A child process is forked
   - In the child process:
     - stdout is redirected to the write end of the pipe
     - `handle_input_redirect("input.txt")` redirects stdin from the file
     - `cat` reads from stdin (the file) and writes to stdout (the pipe)

5. **Right Command Execution**:
   - Another child process is forked
   - In the child process:
     - stdin is redirected to the read end of the pipe
     - `grep` reads from stdin (the pipe) and processes the pattern

6. **Parent Process**:
   - Closes both pipe ends
   - Waits for both child processes to complete
   - Returns the exit status of the right command

### Step-by-Step Example: `cat << EOF ... EOF`

1. **Parsing**:
   - The parser identifies `cat` as the command
   - `<< EOF` is recognized as a heredoc redirection
   - `EOF` is stored as the delimiter

2. **Command Tree Creation**:
   - A simple command node is created
   - Command: `cat`
   - Redirection: heredoc with delimiter `EOF`

3. **Heredoc Processing**:
   - `handle_heredoc("EOF")` is called
   - A pipe is created
   - The shell prompts for input with `> `
   - Each line is written to the pipe until the delimiter is encountered
   - stdin is redirected to read from the pipe

4. **Command Execution**:
   - `cat` reads from stdin (the pipe) and writes to stdout
   - The command returns status 0 (success)

### Handling Multiple Redirections

Multiple redirections are processed in order, with later redirections potentially overriding earlier ones:

```c
// Example: echo hello > file1.txt > file2.txt
// The output will go to file2.txt, not file1.txt

// Example: cat < file1.txt < file2.txt
// The input will come from file2.txt, not file1.txt
```

The implementation handles this by processing redirections sequentially, with each redirection potentially replacing the previous one.

### Error Handling

Errors in redirection setup (e.g., file not found, permission denied) should be reported and prevent command execution:

```c
if (setup_redirections(cmd->redirects) != 0)
{
    // Error occurred during redirection setup
    restore_redirections(saved_fds);
    return 1;
}
```

### Restoring Original File Descriptors

After command execution, original file descriptors should be restored:

```c
int restore_redirections(int saved_fds[2])
{
    // Restore stdin
    dup2(saved_fds[0], STDIN_FILENO);
    close(saved_fds[0]);
    
    // Restore stdout
    dup2(saved_fds[1], STDOUT_FILENO);
    close(saved_fds[1]);
    
    return 0;
}
```

This ensures that redirections are limited to the scope of a single command and don't affect subsequent commands.
