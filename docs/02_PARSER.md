# Parser Component

## Overview

The parser takes the token list produced by the lexer and constructs a structured representation of the command(s) to be executed. It creates command structures with arguments and redirections.

## Example: Processing `echo $'USER'`

### Input Tokens

```
Token: WORD, Value: "echo"
Token: WORD, Value: "$'USER'"
```

### Parser Flow

1. The token list is passed to `parser_tokens()`
2. A command structure is created for the "echo" command
3. Arguments are parsed and added to the command structure
4. Redirections would be parsed if present (none in this example)

### Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `parser_tokens()` | Main entry point for parsing | `cmds = parser_tokens(tokens)` |
| `create_cmd_node()` | Creates a new command node | `cmd = create_cmd_node(&cur_token)` |
| `parse_arguments()` | Parses command arguments | `next_token = parse_arguments(cur_token, cmd)` |
| `add_argument()` | Adds an argument to a command | `add_argument(cmd, "echo", QUOTE_NONE, &argc)` |
| `parse_redirections()` | Parses redirections | `next_token = parse_redirections(cur_token, cmd)` |
| `create_redir()` | Creates a redirection structure | `redir = create_redir(redir_token)` |

### Variables and Data Structures

#### Command Structure
```c
typedef struct s_cmd {
    char **argv;             // Array of command arguments
    t_quote_type *argv_quote; // Quote types for arguments
    t_redir *redirs;         // Linked list of redirections
    int pipe;                // Flag for pipe to next command
    struct s_cmd *next;      // Next command in pipeline
} t_cmd;
```

#### Redirection Structure
```c
typedef struct s_redir {
    char *file;              // Redirection file/target
    int type;                // Redirection type (in, out, append, heredoc)
    bool quoted;             // Whether filename was quoted
    struct s_redir *next;    // Next redirection
} t_redir;
```

### Step-by-Step Processing

For the tokens from `echo $'USER'`:

1. **Command Creation**:
   - Create a new command structure
   - Initialize `argv` array with capacity for arguments
   - Set `redirs` to NULL (no redirections)
   - Set `pipe` to 0 (no pipe to next command)

2. **Argument Parsing**:
   - Process first token: "echo" → add to `argv[0]`
   - Process second token: "$'USER'" → add to `argv[1]`
   - Store quote types for each argument in `argv_quote`

3. **Output**:
   ```
   Command:
   - argv[0]: "echo"
   - argv[1]: "$'USER'"
   - argv_quote[0]: QUOTE_NONE
   - argv_quote[1]: QUOTE_NONE (special handling later)
   - redirs: NULL
   - pipe: 0
   ```

### Source Code Example

```c
t_cmd *parser_tokens(t_token *tokens)
{
    t_cmd *cmd_list = NULL;
    t_cmd *last_cmd = NULL;
    t_token *cur = tokens;
    
    while (cur && cur->type != TOKEN_EOF)
    {
        // Create a new command node
        t_cmd *new_cmd = create_cmd_node(&cur);
        if (!new_cmd)
            break;
        
        // Add to command list
        add_cmd_node(&cmd_list, &last_cmd, new_cmd);
        
        // Check for pipe
        if (cur && cur->type == TOKEN_PIPE)
        {
            last_cmd->pipe = 1;
            cur = cur->next;
        }
    }
    
    return cmd_list;
}

t_token *parse_arguments(t_token *cur, t_cmd *cmd)
{
    int argc = 0;
    
    // Initialize command args
    if (!init_cmd_args(cmd))
        return NULL;
    
    while (cur && cur->type != TOKEN_PIPE && cur->type != TOKEN_EOF)
    {
        if (cur->type == TOKEN_WORD)
        {
            // Process word token into argument
            if (!process_token(cmd, cur, &argc))
                return NULL;
        }
        else if (is_redirection_token(cur->type))
        {
            // Handle redirection (not in this example)
            break;
        }
        
        cur = cur->next;
    }
    
    return cur;
}
```

### Example of Command Structure Creation

For the command `echo $'USER'`:

```c
// Create command structure
t_cmd *cmd = malloc(sizeof(t_cmd));
cmd->argv = malloc(sizeof(char *) * (MAX_ARGS + 1));
cmd->argv_quote = malloc(sizeof(t_quote_type) * MAX_ARGS);
cmd->redirs = NULL;
cmd->pipe = 0;
cmd->next = NULL;

// Add arguments
cmd->argv[0] = strdup("echo");
cmd->argv[1] = strdup("$'USER'");
cmd->argv[2] = NULL;
cmd->argv_quote[0] = QUOTE_NONE;
cmd->argv_quote[1] = QUOTE_NONE; // Will be processed during expansion
```

### Debug Representation

If we were to print the command structure using `print_cmd_list()`, we would see:
```
Command:
  argv[0]: echo
  argv[1]: $'USER'
  Pipe: 0
  No redirections
```
