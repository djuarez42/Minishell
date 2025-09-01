# Minishell Function Reference

This document provides a detailed reference of the key functions in each component of the Minishell project.

## Main Functions

```mermaid
classDiagram
    class MainFunctions {
        +main(int argc, char **argv, char **envp)
        +signals_setup_interactive(t_exec_state *state)
        +new_envp(char **envp)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `main` | Entry point of the program, implements the shell's main loop | `argc`: Argument count<br>`argv`: Argument vector<br>`envp`: Environment variables | `int`: Exit status |
| `signals_setup_interactive` | Sets up signal handlers for interactive mode | `state`: Execution state | `void` |
| `new_envp` | Creates a copy of the environment variables | `envp`: Environment variables | `char **`: New environment |

## Lexer & Tokenizer Functions

```mermaid
classDiagram
    class LexerFunctions {
        +tokenize_input(const char *input)
        +parse_fragments(const char *text)
        +append_fragment(t_fragment **head, t_fragment *frag)
        +new_fragment(const char *text, int len, t_quote_type qtype)
        +create_token(t_fragment *frags, int type)
        +append_token(t_token **tokens, t_token *new)
        +free_token_list(t_token *head)
        +free_fragments(t_fragment *frag)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `tokenize_input` | Main lexer function that converts input string to token list | `input`: Input string | `t_token *`: List of tokens |
| `parse_fragments` | Breaks input into fragments based on quotes and special chars | `text`: Input text | `t_fragment *`: List of fragments |
| `append_fragment` | Adds a new fragment to the fragment list | `head`: Pointer to list head<br>`frag`: Fragment to add | `void` |
| `new_fragment` | Creates a new fragment with the given text and quote type | `text`: Fragment text<br>`len`: Text length<br>`qtype`: Quote type | `t_fragment *`: New fragment |
| `create_token` | Creates a new token with the given fragments and type | `frags`: Token fragments<br>`type`: Token type | `t_token *`: New token |
| `append_token` | Adds a new token to the token list | `tokens`: Pointer to list head<br>`new`: Token to add | `void` |
| `free_token_list` | Frees all memory associated with a token list | `head`: List head | `void` |
| `free_fragments` | Frees all memory associated with a fragment list | `frag`: List head | `void` |

## Parser Functions

```mermaid
classDiagram
    class ParserFunctions {
        +parser_tokens(t_token *tokens)
        +parse_arguments(t_token *cur, t_cmd *cmd)
        +parse_redirections(t_token *cur, t_cmd *cmd)
        +create_redir(t_token *cur)
        +create_cmd_node(t_token **cur)
        +add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
        +free_cmds(t_cmd *cmd)
        +free_redirs(t_redir *redir)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `parser_tokens` | Main parser function that converts tokens to command structures | `tokens`: Token list | `t_cmd *`: Command list |
| `parse_arguments` | Extracts command arguments from tokens | `cur`: Current token<br>`cmd`: Command to fill | `t_token *`: Next token to process |
| `parse_redirections` | Processes redirection tokens | `cur`: Current token<br>`cmd`: Command to add redirections to | `t_token *`: Next token to process |
| `create_redir` | Creates a redirection structure | `cur`: Redirection token | `t_redir *`: New redirection |
| `create_cmd_node` | Creates a new command node | `cur`: Pointer to current token | `t_cmd *`: New command |
| `add_cmd_node` | Adds a command to the command list | `head`: List head<br>`last`: Last node<br>`new_cmd`: Command to add | `void` |
| `free_cmds` | Frees all memory associated with a command list | `cmd`: Command list | `void` |
| `free_redirs` | Frees all memory associated with a redirection list | `redir`: Redirection list | `void` |

## Executor Functions

```mermaid
classDiagram
    class ExecutorFunctions {
        +executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)
        +run_pipeline(t_cmd *start, size_t n_cmds, char **envp, t_exec_state *state)
        +execute_command(char *exec_path, t_cmd *cmd, char **envp)
        +execute_execve(char *exec_path, char **argv, char **envp)
        +handle_redirections(t_redir *redir, char **envp, t_exec_state *state)
        +find_executable(char *cmd, char **envp)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `executor` | Main executor function that handles command execution | `cmd_list`: Command list<br>`penvp`: Pointer to environment<br>`state`: Execution state | `void` |
| `run_pipeline` | Executes a pipeline of commands | `start`: First command<br>`n_cmds`: Number of commands<br>`envp`: Environment<br>`state`: Execution state | `int`: Exit status |
| `execute_command` | Executes a single command | `exec_path`: Executable path<br>`cmd`: Command to execute<br>`envp`: Environment | `int`: Exit status |
| `execute_execve` | Calls execve with proper error handling | `exec_path`: Executable path<br>`argv`: Command arguments<br>`envp`: Environment | `int`: Exit status |
| `handle_redirections` | Sets up file redirections | `redir`: Redirection list<br>`envp`: Environment<br>`state`: Execution state | `int`: Error status |
| `find_executable` | Finds the path to an executable | `cmd`: Command name<br>`envp`: Environment | `char *`: Executable path |

## Environment Variable Functions

```mermaid
classDiagram
    class EnvFunctions {
        +env_get_value(char **envp, const char *name)
        +env_set_var(char ***penvp, const char *name, const char *value)
        +env_unset_var(char ***penvp, const char *name)
        +env_find_index(char **envp, const char *name)
        +expand_variables(const char *input, char **envp, t_exec_state *state, t_quote_type quote)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `env_get_value` | Gets the value of an environment variable | `envp`: Environment<br>`name`: Variable name | `char *`: Variable value |
| `env_set_var` | Sets an environment variable | `penvp`: Pointer to environment<br>`name`: Variable name<br>`value`: Variable value | `int`: Success status |
| `env_unset_var` | Removes an environment variable | `penvp`: Pointer to environment<br>`name`: Variable name | `int`: Success status |
| `env_find_index` | Finds the index of an environment variable | `envp`: Environment<br>`name`: Variable name | `int`: Variable index |
| `expand_variables` | Expands variables in a string | `input`: Input string<br>`envp`: Environment<br>`state`: Execution state<br>`quote`: Quote context | `char *`: Expanded string |

## Heredoc Functions

```mermaid
classDiagram
    class HeredocFunctions {
        +handle_redirections_heredoc(const char *delimiter, bool quoted, char **envp, t_heredoc_args *args)
        +open_heredoc_file(t_heredoc_args *args)
        +write_heredoc_lines(t_heredoc_args *args)
        +redirect_stdin_heredoc(const char *filepath)
        +cleanup_heredoc_file(t_heredoc_args *args)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `handle_redirections_heredoc` | Sets up a heredoc redirection | `delimiter`: Heredoc delimiter<br>`quoted`: Whether delimiter is quoted<br>`envp`: Environment<br>`args`: Heredoc arguments | `int`: Success status |
| `open_heredoc_file` | Creates and opens a temporary file | `args`: Heredoc arguments | `int`: File descriptor |
| `write_heredoc_lines` | Reads lines until delimiter and writes to file | `args`: Heredoc arguments | `int`: Success status |
| `redirect_stdin_heredoc` | Redirects stdin from heredoc file | `filepath`: Path to heredoc file | `void` |
| `cleanup_heredoc_file` | Removes temporary heredoc file | `args`: Heredoc arguments | `void` |

## Built-in Command Functions

```mermaid
classDiagram
    class BuiltinFunctions {
        +is_builtin(const char *cmd)
        +execute_builtin(t_cmd *cmd, char ***penvp, t_exec_state *state)
        +builtin_echo(char **argv)
        +builtin_cd(char **argv, char ***penvp)
        +builtin_pwd(void)
        +builtin_export(char **argv, char ***penvp)
        +builtin_unset(char **argv, char ***penvp)
        +builtin_env(char **envp)
        +builtin_exit(char **argv, t_exec_state *state)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `is_builtin` | Checks if a command is a built-in | `cmd`: Command name | `bool`: Is builtin |
| `execute_builtin` | Executes a built-in command | `cmd`: Command to execute<br>`penvp`: Pointer to environment<br>`state`: Execution state | `int`: Exit status |
| `builtin_echo` | Implements echo command | `argv`: Command arguments | `int`: Exit status |
| `builtin_cd` | Implements cd command | `argv`: Command arguments<br>`penvp`: Pointer to environment | `int`: Exit status |
| `builtin_pwd` | Implements pwd command | None | `int`: Exit status |
| `builtin_export` | Implements export command | `argv`: Command arguments<br>`penvp`: Pointer to environment | `int`: Exit status |
| `builtin_unset` | Implements unset command | `argv`: Command arguments<br>`penvp`: Pointer to environment | `int`: Exit status |
| `builtin_env` | Implements env command | `envp`: Environment | `int`: Exit status |
| `builtin_exit` | Implements exit command | `argv`: Command arguments<br>`state`: Execution state | `int`: Exit status |

## Signal Handling Functions

```mermaid
classDiagram
    class SignalFunctions {
        +signals_setup_interactive(t_exec_state *state)
        +signals_teardown_interactive(void)
        -sigint_handler(int signo)
        -set_echoctl(bool enable)
    }
```

| Function | Description | Parameters | Return Value |
|----------|-------------|------------|--------------|
| `signals_setup_interactive` | Sets up signal handlers for interactive mode | `state`: Execution state | `void` |
| `signals_teardown_interactive` | Restores original signal handlers | None | `void` |
| `sigint_handler` | Handles SIGINT (Ctrl+C) | `signo`: Signal number | `void` |
| `set_echoctl` | Enables/disables control character echo | `enable`: Enable flag | `void` |
