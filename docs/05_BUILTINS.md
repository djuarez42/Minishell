# Builtins Component

## Overview

The builtins component implements shell commands that are built into the shell itself rather than external programs. These commands are executed directly by the shell process without creating a new process. Common builtins include `echo`, `cd`, `pwd`, `export`, `unset`, `env`, and `exit`.

## Example: Processing `echo $'USER'`

### Input Command (After Expansion)

```
echo USER
```

Note: The `$'USER'` has already been expanded to `USER` by the variable expansion component.

### Execution Flow

1. The `execute_builtin()` function determines which builtin to call
2. For `echo`, the `builtin_echo()` function is called
3. `builtin_echo()` processes any options (like `-n`)
4. It then outputs the arguments to stdout
5. Returns an exit status (0 for success)

### Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `execute_builtin()` | Dispatches to the specific builtin | `status = execute_builtin("echo", args, envp, state)` |
| `builtin_echo()` | Implements the echo command | `status = builtin_echo(args)` |
| `builtin_cd()` | Implements the cd command | `status = builtin_cd(args, envp)` |
| `builtin_pwd()` | Implements the pwd command | `status = builtin_pwd()` |
| `builtin_export()` | Implements the export command | `status = builtin_export(args, envp)` |
| `builtin_unset()` | Implements the unset command | `status = builtin_unset(args, envp)` |
| `builtin_env()` | Implements the env command | `status = builtin_env(envp)` |
| `builtin_exit()` | Implements the exit command | `status = builtin_exit(args, state)` |

### Builtin Command Behaviors

| Command | Purpose | Options | Example |
|---------|---------|---------|---------|
| `echo` | Print arguments to stdout | `-n` (no newline) | `echo hello` prints "hello\n" |
| `cd` | Change directory | Path to directory | `cd /home` changes to /home |
| `pwd` | Print working directory | None | `pwd` prints current directory |
| `export` | Set environment variables | Variable assignments | `export VAR=value` sets VAR |
| `unset` | Remove environment variables | Variable names | `unset VAR` removes VAR |
| `env` | Print environment variables | None | `env` shows all variables |
| `exit` | Exit the shell | Exit code (optional) | `exit 1` exits with status 1 |

### Echo Builtin Implementation

The `echo` builtin follows these rules:
1. If the first argument is `-n`, don't print a trailing newline
2. Print all arguments separated by spaces
3. Return 0 (success)

```c
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

### Step-by-Step Processing of `echo USER`

1. **Initial Processing**:
   - `execute_builtin()` is called with command "echo" and arguments ["echo", "USER"]
   - It identifies the command as `echo` and calls `builtin_echo()`

2. **Option Checking**:
   - `builtin_echo()` checks if the first argument is `-n`
   - For our example, it's "USER", so no options are present
   - The `print_newline` flag remains set to 1

3. **Argument Printing**:
   - The function iterates through the arguments starting at index 1
   - It prints "USER" to stdout
   - Since there are no more arguments, no spaces are printed

4. **Newline Handling**:
   - Since `print_newline` is 1, a newline character is printed
   - The final output is "USER\n"

5. **Return Status**:
   - The function returns 0 (success)
   - This status is stored in the execution state for `$?`

### Source Code Example

```c
int execute_builtin(const char *cmd, char **args, char **envp, t_exec_state *state)
{
    if (ft_strcmp(cmd, "echo") == 0)
        return builtin_echo(args);
    else if (ft_strcmp(cmd, "cd") == 0)
        return builtin_cd(args, envp);
    else if (ft_strcmp(cmd, "pwd") == 0)
        return builtin_pwd();
    else if (ft_strcmp(cmd, "export") == 0)
        return builtin_export(args, envp);
    else if (ft_strcmp(cmd, "unset") == 0)
        return builtin_unset(args, envp);
    else if (ft_strcmp(cmd, "env") == 0)
        return builtin_env(envp);
    else if (ft_strcmp(cmd, "exit") == 0)
        return builtin_exit(args, state);
    
    return 1;  // Command not found
}

// Implementation of other builtins

int builtin_cd(char **args, char **envp)
{
    char *path;
    char cwd[PATH_MAX];
    
    // Get target directory
    if (!args[1] || ft_strcmp(args[1], "~") == 0)
        path = env_get_value(envp, "HOME");
    else
        path = args[1];
    
    // Change directory
    if (chdir(path) != 0)
    {
        ft_putstr_fd("cd: ", STDERR_FILENO);
        ft_putstr_fd(path, STDERR_FILENO);
        ft_putstr_fd(": ", STDERR_FILENO);
        ft_putendl_fd(strerror(errno), STDERR_FILENO);
        return 1;
    }
    
    // Update PWD environment variable
    if (getcwd(cwd, PATH_MAX))
    {
        // Code to update PWD in environment
    }
    
    return 0;
}

int builtin_pwd(void)
{
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, PATH_MAX))
    {
        ft_putendl_fd(cwd, STDOUT_FILENO);
        return 0;
    }
    else
    {
        ft_putendl_fd(strerror(errno), STDERR_FILENO);
        return 1;
    }
}

int builtin_export(char **args, char **envp)
{
    int i = 1;
    int status = 0;
    
    if (!args[1])
    {
        // Print sorted environment with declare -x prefix
        print_sorted_env(envp);
        return 0;
    }
    
    while (args[i])
    {
        if (is_valid_identifier(args[i]))
        {
            // Add or update environment variable
            env_set(envp, args[i]);
        }
        else
        {
            ft_putstr_fd("export: '", STDERR_FILENO);
            ft_putstr_fd(args[i], STDERR_FILENO);
            ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
            status = 1;
        }
        i++;
    }
    
    return status;
}

int builtin_exit(char **args, t_exec_state *state)
{
    int exit_code = state->last_status;
    
    ft_putendl_fd("exit", STDERR_FILENO);
    
    if (args[1])
    {
        if (is_numeric(args[1]))
        {
            exit_code = ft_atoi(args[1]) & 255;
            
            if (args[2])
            {
                ft_putendl_fd("exit: too many arguments", STDERR_FILENO);
                return 1;
            }
        }
        else
        {
            ft_putstr_fd("exit: ", STDERR_FILENO);
            ft_putstr_fd(args[1], STDERR_FILENO);
            ft_putendl_fd(": numeric argument required", STDERR_FILENO);
            exit_code = 255;
        }
    }
    
    // Perform cleanup and exit
    // cleanup_shell();
    exit(exit_code);
    
    return exit_code;  // Never reached
}
```

### Exit Status Handling

All builtins return an exit status:
- 0: Success
- Non-zero: Failure

This status is stored in the shell's state and can be accessed with `$?`.

For `echo USER`:
1. `builtin_echo()` returns 0
2. This is stored in `state->last_status`
3. A subsequent command using `$?` would get "0"

### Builtin vs External Command Determination

The shell determines if a command is a builtin using a simple string comparison:

```c
int is_builtin(const char *cmd)
{
    return (
        ft_strcmp(cmd, "echo") == 0 ||
        ft_strcmp(cmd, "cd") == 0 ||
        ft_strcmp(cmd, "pwd") == 0 ||
        ft_strcmp(cmd, "export") == 0 ||
        ft_strcmp(cmd, "unset") == 0 ||
        ft_strcmp(cmd, "env") == 0 ||
        ft_strcmp(cmd, "exit") == 0
    );
}
```

If the command is a builtin, it's executed directly. Otherwise, it's treated as an external command and executed using `fork()` and `execve()`.
