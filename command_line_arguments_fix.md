# Command-Line Arguments Fix for Minishell

## Problem Description

The minishell program wasn't properly handling command-line arguments, which prevented it from executing commands passed directly to the program. For example, running:

```bash
./minishell "e""c""h""o" hola
```

wasn't working as expected. The program was ignoring any command-line arguments provided.

## Old Code

Previously, in `main.c`, the program was explicitly ignoring command-line arguments with these lines:

```c
int main(int argc, char **argv, char **envp)
{
    t_exec_state state;
    char *input;
    t_token *tokens;
    t_cmd *cmds;
    char **envp_copy;
    t_cmd *cur;
    int fail;

    (void)argc;
    (void)argv;

    envp_copy = new_envp(envp);
    if (!envp_copy)
        return (1);

    state = (t_exec_state){0};
    if (isatty(STDIN_FILENO))
        signals_setup_interactive(&state);
    
    // ... rest of function ...
}
```

The `(void)argc;` and `(void)argv;` lines were explicitly telling the compiler that these arguments would not be used, and the program would only enter interactive mode regardless of whether command-line arguments were provided.

## New Code

The updated version of the code now properly handles command-line arguments:

```c
int main(int argc, char **argv, char **envp)
{
    t_exec_state state;
    char *input;
    t_token *tokens;
    t_cmd *cmds;
    char **envp_copy;
    t_cmd *cur;
    int fail;

    envp_copy = new_envp(envp);
    if (!envp_copy)
        return (1);

    state = (t_exec_state){0};
    
    // If command line arguments were provided (argc > 1), execute them directly
    if (argc > 1)
    {
        // Join arguments with spaces to create a command string
        char *cmd_str = NULL;
        for (int i = 1; i < argc; i++)
        {
            if (i > 1)
                cmd_str = ft_strjoin_free(cmd_str, " ");
            cmd_str = ft_strjoin_free(cmd_str, argv[i]);
        }
        
        if (cmd_str)
        {
            tokens = tokenize_input(cmd_str);
            if (tokens)
            {
                cmds = parser_tokens(tokens);
                if (cmds)
                {
                    cur = cmds;
                    fail = 0;
                    while (cur)
                    {
                        if (expand_cmd_inplace(cur, envp_copy, &state) == -1)
                        {
                            fail = 1;
                            break;
                        }
                        cur = cur->next;
                    }
                    
                    if (!fail)
                        executor(cmds, &envp_copy, &state);
                    
                    free_cmds(cmds);
                }
                free_token_list(tokens);
            }
            free(cmd_str);
        }
        
        free_envp(envp_copy);
        return (state.last_status);
    }
    
    if (isatty(STDIN_FILENO))
        signals_setup_interactive(&state);
    
    // ... rest of function for interactive mode ...
}
```

## Changes Explained

1. **Removed Unused Variable Casting**:
   - Removed `(void)argc;` and `(void)argv;` to allow use of command-line arguments.

2. **Added Command-Line Argument Processing**:
   - Added a conditional block that executes when `argc > 1`.
   - Implemented a loop to join all arguments into a single command string.
   - Used the same pipeline (tokenize, parse, expand, execute) as in interactive mode.
   - Properly handled cleanup and returned the appropriate exit status.

3. **Flow Control**:
   - The program now has two distinct execution paths:
     1. Command-line mode: When arguments are provided, execute them and exit.
     2. Interactive mode: When no arguments are provided, run the interactive shell.

## Testing Results

The following tests now work correctly:

1. **Adjacent quoted strings**:
   ```bash
   ./minishell "e""c""h""o" hola
   ```
   Output: `hola`

2. **Single quoted commands**:
   ```bash
   ./minishell 'echo cd ~'
   ```
   Output: `cd ~`

3. **Nested quotes**:
   ```bash
   ./minishell 'echo "cd ~"'
   ```
   Output: `cd ~`

4. **Variable expansion**:
   ```bash
   ./minishell 'echo $USER'
   ```
   Output: `ekakhmad`

This fix ensures that the minishell program properly handles both interactive and command-line modes, similar to how the real Bash shell operates.
