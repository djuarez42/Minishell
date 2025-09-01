# Environment Handling Component

## Overview

The environment handling component manages environment variables throughout the shell's operation. It provides functions for getting, setting, and unsetting environment variables, as well as creating a clean environment for child processes. Environment variables are crucial for shell operation, affecting everything from command lookup to user customization.

## Example: Processing Environment Variables

While our example command `echo $'USER'` doesn't directly interact with environment variables (since `$'USER'` is an ANSI-C quoted string, not a variable reference), the environment is still integral to the shell's operation. Let's examine how environment variables are managed.

### Environment Management Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `env_init()` | Initializes the environment array | `envp = env_init(environ)` |
| `env_get_value()` | Gets value of an environment variable | `value = env_get_value(envp, "USER")` |
| `env_set()` | Sets an environment variable | `env_set(envp, "VAR=value")` |
| `env_unset()` | Removes an environment variable | `env_unset(envp, "VAR")` |
| `env_copy()` | Creates a copy of the environment | `new_env = env_copy(envp)` |
| `env_free()` | Frees the environment array | `env_free(envp)` |

### Variables and Data Structures

The environment is stored as a NULL-terminated array of strings in the format "NAME=value":

```c
// Environment array
char **envp;  // NULL-terminated array of "NAME=value" strings
```

### Environment Initialization

The shell initializes its environment by copying the parent environment:

```c
char **env_init(char **parent_env)
{
    char **new_env;
    int count = 0;
    int i;
    
    // Count environment variables
    while (parent_env[count])
        count++;
        
    // Allocate space for new environment (plus NULL terminator)
    new_env = (char **)malloc(sizeof(char *) * (count + 1));
    if (!new_env)
        return NULL;
        
    // Copy environment variables
    i = 0;
    while (i < count)
    {
        new_env[i] = ft_strdup(parent_env[i]);
        if (!new_env[i])
        {
            // Cleanup on error
            while (--i >= 0)
                free(new_env[i]);
            free(new_env);
            return NULL;
        }
        i++;
    }
    
    // NULL terminate
    new_env[count] = NULL;
    
    return new_env;
}
```

### Getting Environment Variable Values

The `env_get_value()` function retrieves the value of an environment variable:

```c
char *env_get_value(char **envp, const char *name)
{
    int i = 0;
    int name_len = ft_strlen(name);
    
    while (envp[i])
    {
        // Check if this entry starts with name followed by '='
        if (ft_strncmp(envp[i], name, name_len) == 0 && 
            envp[i][name_len] == '=')
        {
            // Return pointer to the value part (after '=')
            return envp[i] + name_len + 1;
        }
        i++;
    }
    
    return NULL;  // Variable not found
}
```

### Setting Environment Variables

The `env_set()` function sets or updates an environment variable:

```c
int env_set(char ***envp, const char *var_string)
{
    char *equal_sign;
    char *name;
    int name_len;
    int i = 0;
    
    // Find the '=' in the string
    equal_sign = ft_strchr(var_string, '=');
    if (!equal_sign)
        return 1;  // Invalid format
        
    // Extract the variable name
    name_len = equal_sign - var_string;
    name = ft_strndup(var_string, name_len);
    if (!name)
        return 1;
        
    // Check if variable already exists
    while ((*envp)[i])
    {
        if (ft_strncmp((*envp)[i], name, name_len) == 0 && 
            (*envp)[i][name_len] == '=')
        {
            // Replace existing variable
            free((*envp)[i]);
            (*envp)[i] = ft_strdup(var_string);
            free(name);
            return (*envp)[i] ? 0 : 1;
        }
        i++;
    }
    
    // Variable doesn't exist, add it
    *envp = env_add_var(*envp, var_string);
    free(name);
    
    return *envp ? 0 : 1;
}

char **env_add_var(char **envp, const char *var_string)
{
    char **new_env;
    int count = 0;
    
    // Count current environment size
    while (envp[count])
        count++;
        
    // Allocate new environment with space for one more variable
    new_env = (char **)malloc(sizeof(char *) * (count + 2));
    if (!new_env)
        return NULL;
        
    // Copy existing variables
    for (int i = 0; i < count; i++)
        new_env[i] = envp[i];
        
    // Add new variable
    new_env[count] = ft_strdup(var_string);
    if (!new_env[count])
    {
        free(new_env);
        return envp;  // Return original on error
    }
    
    // NULL terminate
    new_env[count + 1] = NULL;
    
    free(envp);  // Free old array
    return new_env;
}
```

### Unsetting Environment Variables

The `env_unset()` function removes an environment variable:

```c
int env_unset(char ***envp, const char *name)
{
    int i = 0;
    int j;
    int name_len = ft_strlen(name);
    int count = 0;
    
    // Count environment variables
    while ((*envp)[count])
        count++;
        
    // Find the variable
    while ((*envp)[i])
    {
        if (ft_strncmp((*envp)[i], name, name_len) == 0 && 
            (*envp)[i][name_len] == '=')
        {
            // Free the variable string
            free((*envp)[i]);
            
            // Shift all subsequent variables down
            j = i;
            while (j < count)
            {
                (*envp)[j] = (*envp)[j + 1];
                j++;
            }
            
            return 0;  // Success
        }
        i++;
    }
    
    return 1;  // Variable not found
}
```

### Environment Usage in Command Execution

When executing commands, the environment is passed to child processes:

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
        execve(cmd_path, args, envp);  // Pass environment to new process
        exit(127);  // Command execution failed
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

### Finding Command Paths

The `PATH` environment variable is used to locate executable commands:

```c
char *find_command_path(const char *cmd, char **envp)
{
    char *path_env;
    char **paths;
    char *cmd_path = NULL;
    int i = 0;
    
    // If command contains a slash, it's a direct path
    if (ft_strchr(cmd, '/'))
        return ft_strdup(cmd);
        
    // Get PATH environment variable
    path_env = env_get_value(envp, "PATH");
    if (!path_env)
        return NULL;
        
    // Split PATH into individual directories
    paths = ft_split(path_env, ':');
    if (!paths)
        return NULL;
        
    // Try each directory in PATH
    while (paths[i])
    {
        cmd_path = ft_strjoin3(paths[i], "/", cmd);
        if (!cmd_path)
        {
            free_string_array(paths);
            return NULL;
        }
        
        // Check if file exists and is executable
        if (access(cmd_path, X_OK) == 0)
        {
            free_string_array(paths);
            return cmd_path;
        }
        
        free(cmd_path);
        i++;
    }
    
    free_string_array(paths);
    return NULL;  // Command not found in PATH
}
```

### Environment Variables in Builtins

Environment variables are used extensively in builtin commands:

1. **cd**: Uses `HOME` if no argument is provided
2. **pwd**: May use `PWD` if available
3. **export**: Adds/updates environment variables
4. **unset**: Removes environment variables
5. **env**: Displays all environment variables

For example, the `env` builtin simply prints the environment:

```c
int builtin_env(char **envp)
{
    int i = 0;
    
    while (envp[i])
    {
        ft_putendl_fd(envp[i], STDOUT_FILENO);
        i++;
    }
    
    return 0;
}
```

### Special Variables

The shell maintains some special variables that aren't part of the environment:

1. **$?**: Last command exit status
2. **$$**: Current process ID
3. **$0**: Shell name

These are handled separately during variable expansion:

```c
char *expand_special_var(const char *var, t_exec_state *state)
{
    if (ft_strcmp(var, "?") == 0)
    {
        // Last exit status
        return ft_itoa(state->last_status);
    }
    else if (ft_strcmp(var, "$") == 0)
    {
        // Current process ID
        return ft_itoa(getpid());
    }
    else if (ft_strcmp(var, "0") == 0)
    {
        // Shell name
        return ft_strdup("minishell");
    }
    
    return NULL;  // Not a special variable
}
```

### Environment Impact on Our Example

While `echo $'USER'` doesn't directly use environment variables, the shell's operation depends on them:

1. Finding the `echo` command if it were external would use `PATH`
2. The shell prompt may use `PS1`, `USER`, or `HOSTNAME`
3. Command history may use `HISTFILE` and `HISTSIZE`

The environment provides the context in which all shell commands operate.
