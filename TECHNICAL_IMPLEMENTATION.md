# Minishell Technical Implementation Guide

This document provides a detailed technical explanation of how the Minishell project is implemented, with code examples and flow diagrams for each major component.

## Main Loop Implementation

The shell's main loop in `main.c` follows this pattern:

```c
int main(int argc, char **argv, char **envp)
{
    t_exec_state state;
    char *input;
    t_token *tokens;
    t_cmd *cmds;
    char **envp_copy;

    // Initialize environment and signals
    signals_setup_interactive(&state);
    envp_copy = new_envp(envp);
    state.last_status = 0;
    
    // Main shell loop
    while (1)
    {
        // Read input
        input = readline("minishell$ ");
        if (!input)
        {
            printf("exit\n");
            break;
        }
        
        // Add to history if non-empty
        if (*input)
            add_history(input);
            
        // Tokenize input
        tokens = tokenize_input(input);
        if (!tokens)
        {
            free(input);
            continue;
        }
        
        // Parse tokens into command structures
        cmds = parser_tokens(tokens);
        
        // Execute commands if valid
        if (cmds && valid_commands(cmds))
            executor(cmds, &envp_copy, &state);
            
        // Cleanup
        free_token_list(tokens);
        free_cmds(cmds);
        free(input);
    }
    
    // Final cleanup
    free_envp(envp_copy);
    return (0);
}
```

## Lexer & Tokenizer: Deep Dive

### Fragment-Based Approach

The lexer uses a fragment-based approach that provides flexibility for handling quotes and compound tokens. This approach treats each input segment as a fragment with a quote type, which can later be combined into tokens.

**Core Functions:**

1. **parse_fragments**: Breaks input into fragments based on quotes and operators

```c
t_fragment *parse_fragments(const char *input)
{
    t_fragment *fragments = NULL;
    int i = 0;
    
    while (input[i])
    {
        // Skip spaces between fragments
        if (lx_is_space(input[i]))
        {
            i++;
            continue;
        }
        
        // Create new fragment based on quote type
        if (input[i] == '\'')
            add_quoted_fragment(&fragments, input, &i, QUOTE_SINGLE);
        else if (input[i] == '\"')
            add_quoted_fragment(&fragments, input, &i, QUOTE_DOUBLE);
        else if (lx_is_meta(input[i]))
            add_operator_fragment(&fragments, input, &i);
        else
            add_text_fragment(&fragments, input, &i);
    }
    
    return fragments;
}
```

2. **tokenize_input**: Combines fragments into meaningful tokens

```c
t_token *tokenize_input(const char *input)
{
    t_fragment *frags = parse_fragments(input);
    t_token *tokens = NULL;
    
    // Process fragments into tokens
    while (frags)
    {
        int type = determine_token_type(frags);
        t_token *new_token = create_token(frags, type);
        
        // Move to next fragment set
        frags = get_next_fragment_set(frags);
        
        // Add token to list
        append_token(&tokens, new_token);
    }
    
    return tokens;
}
```

### Quote Handling

The lexer distinguishes between different types of quotes:

- **Single quotes ('text')**: No variable expansion, literal interpretation
- **Double quotes ("text")**: Variable expansion allowed
- **ANSI-C quotes ($'text')**: Special character escaping (e.g., $'\\n' becomes newline)
- **Locale-specific quotes ($"text")**: Locale-dependent translations

## Parser Implementation

### Command Structure Building

The parser converts token lists into executable command structures:

```c
t_cmd *parser_tokens(t_token *tokens)
{
    t_cmd *cmd_list = NULL;
    t_cmd *last_cmd = NULL;
    t_token *cur = tokens;
    
    while (cur && cur->type != TOKEN_EOF)
    {
        // Create new command node
        t_cmd *new_cmd = create_cmd_node(&cur);
        
        if (!new_cmd)
            break;
            
        // Add to command list
        add_cmd_node(&cmd_list, &last_cmd, new_cmd);
        
        // Skip pipe if present
        if (cur && cur->type == TOKEN_PIPE)
        {
            last_cmd->pipe = 1;
            cur = cur->next;
        }
    }
    
    return cmd_list;
}
```

### Redirection Parsing

```c
t_token *parse_redirections(t_token *cur, t_cmd *cmd)
{
    t_redir *new_redir;
    
    // Create redirection structure
    new_redir = create_redir(cur);
    
    // Add to redirection list
    if (!cmd->redirs)
        cmd->redirs = new_redir;
    else
    {
        // Append to end of list
        t_redir *last = cmd->redirs;
        while (last->next)
            last = last->next;
        last->next = new_redir;
    }
    
    // Move past redirection operator and filename
    return cur->next->next;
}
```

## Executor Mechanics

### Pipeline Execution

```c
static int run_pipeline(t_cmd *start, size_t n_cmds, char **envp, t_exec_state *state)
{
    int (*pipes)[2] = NULL;
    size_t n_pipes = (n_cmds > 1) ? n_cmds - 1 : 0;
    pid_t *pids;
    
    // Create necessary pipes
    if (create_pipes(&pipes, n_pipes) == -1)
        return 1;
        
    // Allocate process ID array
    pids = malloc(sizeof(pid_t) * n_cmds);
    
    // Execute each command in pipeline
    t_cmd *cur = start;
    for (size_t i = 0; i < n_cmds; i++)
    {
        pids[i] = fork();
        
        if (pids[i] == 0)
        {
            // Child process
            
            // Setup redirections between pipe commands
            wire_child_pipes(i, n_cmds, pipes);
            
            // Close all pipes in child
            if (pipes)
                close_all_pipes(pipes, n_pipes);
                
            // Handle redirections and execute
            handle_redirections_and_quotes(cur->redirs, envp, state);
            
            // Execute command
            if (is_builtin(cur->argv[0]))
                exit(run_builtin_in_child(cur, &envp));
            else
                exit(execute_command(NULL, cur, envp));
        }
        
        cur = cur->next;
    }
    
    // Close all pipes in parent
    if (pipes)
        close_all_pipes(pipes, n_pipes);
        
    // Wait for all processes to complete
    int status = wait_pipeline(pids, n_cmds);
    
    // Clean up
    free(pipes);
    free(pids);
    
    return status;
}
```

### Redirection Implementation

```c
int handle_redirections(t_redir *redir, char **envp, t_exec_state *state)
{
    while (redir)
    {
        // Expand variables in redirection targets if not quoted
        char *filename = redir->file;
        if (!redir->quoted)
            filename = expand_variables(redir->file, envp, state, QUOTE_NONE);
            
        // Apply redirection based on type
        switch (redir->type)
        {
            case TOKEN_REDIRECT_IN:
                handle_redirections_in(filename, &error);
                break;
            case TOKEN_REDIRECT_OUT:
                handle_redirections_out(filename, &error);
                break;
            case TOKEN_APPEND:
                handle_redirections_append(filename, &error);
                break;
            case TOKEN_HEREDOC:
                handle_redirections_heredoc(filename, redir->quoted, envp, &heredoc_args);
                break;
        }
        
        // Clean up and move to next redirection
        if (!redir->quoted)
            free(filename);
        redir = redir->next;
    }
    
    return 0;
}
```

## Environment Variable Management

### Variable Expansion

```c
char *expand_variables(const char *input, char **envp, t_exec_state *state, t_quote_type quote)
{
    char *result = NULL;
    int i = 0;
    
    while (input[i])
    {
        // Handle $ for variable expansion
        if (input[i] == '$' && quote != QUOTE_SINGLE)
        {
            i++;  // Skip $
            
            // Special variables
            if (input[i] == '?')
            {
                // Expand $? to last exit status
                char *status_str = expand_exit_status(state);
                result = ft_strjoin_free(result, status_str);
                free(status_str);
                i++;
            }
            else if (is_var_char(input[i]))
            {
                // Extract variable name
                int var_len = 0;
                while (is_var_char(input[i + var_len]))
                    var_len++;
                    
                // Get name and value
                char *var_name = ft_substr(input, i, var_len);
                char *var_value = env_get_value(envp, var_name);
                
                // Add value to result
                result = ft_strjoin_free(result, var_value ? var_value : "");
                
                // Clean up and advance
                free(var_name);
                i += var_len;
            }
        }
        else
        {
            // Add regular character
            char tmp[2] = {input[i], '\0'};
            result = ft_strjoin_free(result, tmp);
            i++;
        }
    }
    
    return result;
}
```

### ANSI-C Quote Handling

```c
char *handle_ansi_c_quotes(const char *input, int *i)
{
    char *result = ft_strdup("");
    
    // Skip $' prefix
    (*i) += 2;
    
    // Process until closing quote
    while (input[*i] && input[*i] != '\'')
    {
        if (input[*i] == '\\')
        {
            (*i)++;  // Skip backslash
            
            // Handle escape sequences
            switch (input[*i])
            {
                case 'n': result = ft_strjoin_free(result, "\n"); break;
                case 't': result = ft_strjoin_free(result, "\t"); break;
                case 'r': result = ft_strjoin_free(result, "\r"); break;
                // ... other escape sequences
                default: 
                    // Add literal character
                    char tmp[2] = {input[*i], '\0'};
                    result = ft_strjoin_free(result, tmp);
            }
        }
        else
        {
            // Add regular character
            char tmp[2] = {input[*i], '\0'};
            result = ft_strjoin_free(result, tmp);
        }
        
        (*i)++;
    }
    
    // Skip closing quote
    if (input[*i] == '\'')
        (*i)++;
        
    return result;
}
```

## Heredoc Implementation

```c
int handle_redirections_heredoc(const char *delimiter, bool quoted, char **envp, t_heredoc_args *args)
{
    // Create unique temporary file
    args->heredoc_path = ft_strjoin("/tmp/minishell_heredoc_", random_string(8));
    
    // Open file for writing
    args->fd = open(args->heredoc_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (args->fd == -1)
        return 1;
        
    // Store arguments
    args->delimiter = delimiter;
    args->quoted = quoted;
    args->envp = envp;
    
    // Read heredoc content
    int result = write_heredoc_lines(args);
    
    // Redirect stdin to heredoc file
    if (result == 0)
    {
        close(args->fd);
        redirect_stdin_heredoc(args->heredoc_path);
    }
    
    // Clean up temp file when done
    cleanup_heredoc_file(args);
    
    return result;
}
```

## Error Handling and Exit Codes

```c
void executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)
{
    t_cmd *cur = cmd_list;
    
    while (cur)
    {
        // Check for command validity
        if (!cur->argv || !cur->argv[0])
        {
            fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
            state->last_status = 2;
            cur = cur->next;
            continue;
        }
        
        // Handle different execution scenarios
        if (is_builtin(cur->argv[0]) && !cur->pipe && (!cur->next || !cur->next->pipe))
        {
            // Execute builtin directly in parent process
            state->last_status = execute_builtin(cur, penvp, state);
        }
        else
        {
            // Execute in pipeline
            size_t n_cmds = count_pipeline_cmds(cur);
            state->last_status = run_pipeline(cur, n_cmds, *penvp, state);
            
            // Skip to end of pipeline
            while (n_cmds > 1 && cur)
            {
                cur = cur->next;
                n_cmds--;
            }
        }
        
        if (cur)
            cur = cur->next;
    }
}
```

## Signal Handling

```c
static void sigint_handler(int signo)
{
    (void)signo;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_redisplay();
    if (g_state_ptr)
        g_state_ptr->last_status = 130;
}

void signals_setup_interactive(t_exec_state *state)
{
    struct sigaction sa;
    
    g_state_ptr = state;
    
    // Disable ^C display in terminal
    if (isatty(STDIN_FILENO))
    {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ECHOCTL;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
    
    // Set up SIGINT handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    
    // Ignore SIGQUIT (^\\)
    signal(SIGQUIT, SIG_IGN);
}
```

## Output Handling Issues

The current implementation faces issues with command output display:

1. **Problem**: Commands executed in the minishell don't show their output
2. **Possible Causes**:
   - File descriptor handling in child processes
   - Signal handling interfering with output
   - Terminal attribute configuration
   - Output buffering issues

A potential fix would involve reviewing the executor implementation to ensure proper handling of file descriptors and output buffering.

## Conclusion

This technical guide provides an in-depth look at the implementation details of Minishell. Understanding these components is crucial for maintaining and extending the project. The modular architecture allows for targeted improvements in specific areas without affecting the entire codebase.
