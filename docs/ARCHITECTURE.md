# Minishell Architecture Documentation

## Table of Contents
1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Core Components](#core-components)
4. [Data Flow](#data-flow)
5. [Module Organization](#module-organization)
6. [Key Data Structures](#key-data-structures)
7. [Execution Pipeline](#execution-pipeline)
8. [Error Handling](#error-handling)
9. [Memory Management](#memory-management)
10. [Testing Framework](#testing-framework)

---

## Overview

Minishell is a simplified Unix shell implementation that follows a modular architecture with clear separation of concerns. The shell processes user input through a well-defined pipeline: **Lexical Analysis → Parsing → Command Expansion → Execution**.

### Design Philosophy
- **Modular Design**: Each component has a specific responsibility
- **Clear Interfaces**: Well-defined APIs between modules
- **Memory Safety**: Proper allocation and cleanup throughout
- **Extensibility**: Easy to add new features and builtins
- **POSIX Compliance**: Follows shell standards where applicable

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      MINISHELL ARCHITECTURE                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     │
│  │    USER     │───▶│   MAIN.C    │───▶│   SIGNALS   │     │
│  │   INPUT     │    │  (Control)  │    │ (Handling)  │     │
│  └─────────────┘    └─────────────┘    └─────────────┘     │
│         │                   │                              │
│         ▼                   ▼                              │
│  ┌─────────────┐    ┌─────────────┐                       │
│  │   LEXER     │───▶│   PARSER    │                       │
│  │ (Tokenize)  │    │ (Commands)  │                       │
│  └─────────────┘    └─────────────┘                       │
│         │                   │                              │
│         ▼                   ▼                              │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     │
│  │  EXECUTOR   │───▶│  BUILTINS   │───▶│ ENVIRONMENT │     │
│  │ (Commands)  │    │ (Internal)  │    │ (Variables) │     │
│  └─────────────┘    └─────────────┘    └─────────────┘     │
│         │                                                  │
│         ▼                                                  │
│  ┌─────────────┐                                          │
│  │   LIBFT     │                                          │
│  │ (Utilities) │                                          │
│  └─────────────┘                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Core Components

### 1. Main Control (`main.c`)
**Purpose**: Application entry point and main execution loop

**Key Functions**:
- `main()` - Entry point with command-line argument handling
- Input mode detection (interactive vs. batch vs. -c option)
- Main REPL (Read-Eval-Print Loop)
- Integration of all subsystems

**Responsibilities**:
```c
// Interactive mode detection
if (isatty(fileno(stdin)))
    input = readline("minishell$ ");
else
    input = get_next_line(fileno(stdin));

// Pipeline execution
tokens = tokenize_input(input);           // Lexer
cmds = parser_tokens(tokens);             // Parser  
expand_cmd_inplace(cmds, envp, &state);   // Expansion
execute_commands(cmds, envp, &state);     // Execution
```

### 2. Lexer Module (`src/lexer/`)
**Purpose**: Convert raw input string into structured tokens

**File Structure**:
```
src/lexer/
├── tokenizer.c           # Main tokenization entry point
├── lexer_utils1.c        # Fragment parsing utilities
├── lexer_utils2.c        # Quote and escape handling
├── lexer_utils3.c        # Token classification
├── lexer_utils4.c        # Token validation
└── lexer_utils5.c        # Token list management
```

**Key Data Flow**:
```
Raw Input → Fragments → Tokens → Validated Token List
    ↓           ↓         ↓            ↓
"echo 'hi'"  [echo][hi]  TOKEN_WORD   Final tokens
```

**Responsibilities**:
- Quote parsing (single, double, mixed)
- Escape sequence handling
- Word boundary detection
- Operator recognition (`|`, `>`, `>>`, `<`, `<<`)
- Token type classification

### 3. Parser Module (`src/parser/`)
**Purpose**: Convert tokens into executable command structures

**File Structure**:
```
src/parser/
├── parser.c              # Main parsing logic
├── parser_utils.c        # Command structure building
├── parser_utils2.c       # Redirection handling
├── parser_print_utils.c  # Debug output
└── parser_errors.c       # Syntax error detection
```

**Key Data Flow**:
```
Tokens → Command Structures → Validated Commands
  ↓            ↓                    ↓
[echo][hi]   t_cmd{args, redirs}   Ready for execution
```

**Responsibilities**:
- Command argument assembly
- Redirection parsing (`>`, `>>`, `<`, `<<`)
- Pipeline construction
- Syntax validation
- Error reporting

### 4. Executor Module (`src/executor/`)
**Purpose**: Execute parsed commands with proper environment handling

**File Structure**:
```
src/executor/
├── executor.c                    # Main execution engine
├── executor_utils.c              # Process management
├── env_utils.c                   # Environment operations
├── expand_variables.c            # Variable expansion
├── expand_variables_utils.c      # Expansion utilities
├── expand_variables_utils2.c     # Advanced expansion
├── expand_mixed_quotes.c         # Quote/variable mixing
├── expand_cmd_inplace.c          # Command preprocessing
├── preprocess_dollar_quotes.c    # Dollar quote handling
├── dollar_string_fix.c           # Dollar syntax fixes
├── dollar_quotes_fix.c           # Dollar quote processing
├── redir_utils.c                 # Redirection handling
├── redir_utils2.c                # Advanced redirections
└── heredoc_utils.c               # Here-document processing
```

**Key Responsibilities**:
- Variable expansion (`$USER`, `$?`, `$$`)
- Quote processing and removal
- Command execution (fork/exec)
- Pipeline management
- Redirection setup
- Built-in vs external command routing

### 5. Builtins Module (`src/builtins/`)
**Purpose**: Implement shell built-in commands

**File Structure**:
```
src/builtins/
├── builtins.c                # Built-in dispatcher
├── builtin_echo_pwd_env.c    # echo, pwd, env commands
├── builtin_cd.c              # cd command with complex logic
├── builtin_export.c          # export command
└── builtin_unset_exit.c      # unset, exit commands
```

**Built-in Commands**:
- `echo` - Text output with -n flag support
- `cd` - Directory navigation with error handling
- `pwd` - Current directory display
- `export` - Environment variable management
- `unset` - Variable removal
- `env` - Environment display
- `exit` - Shell termination

### 6. Environment Module (`src/env/`)
**Purpose**: Manage environment variables and shell state

**Responsibilities**:
- Environment variable storage
- Variable lookup and modification
- Export/unset operations
- Exit status tracking

### 7. Signals Module (`src/signals/`)
**Purpose**: Handle Unix signals appropriately for shell behavior

**Key Signals**:
- `SIGINT` (Ctrl+C) - Interrupt current command
- `SIGQUIT` (Ctrl+\\) - Quit with core dump
- `EOF` (Ctrl+D) - End of input

---

## Data Flow

### Complete Processing Pipeline

```
┌─────────────────┐
│   User Input    │
│  "echo 'hello'" │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐    LEXER PHASE
│   Tokenizer     │    ═════════════
│ tokenize_input()│
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Fragments     │
│ parse_fragments │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│     Tokens      │
│ TOKEN_WORD list │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐    PARSER PHASE
│     Parser      │    ════════════
│ parser_tokens() │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Commands      │
│ t_cmd structure │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐    EXPANSION PHASE
│   Expansion     │    ═══════════════
│expand_cmd_inplace│
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Expanded Cmds   │
│ Variables → Values│
└─────────┬───────┘
          │
          ▼
┌─────────────────┐    EXECUTION PHASE
│   Executor      │    ═══════════════
│ execute_commands│
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│    Output       │
│   "hello"       │
└─────────────────┘
```

---

## Key Data Structures

### Token Structure (`include/lexer.h`)
```c
typedef enum {
    TOKEN_WORD,      // Regular words and strings
    TOKEN_PIPE,      // |
    TOKEN_REDIR_IN,  // <
    TOKEN_REDIR_OUT, // >
    TOKEN_APPEND,    // >>
    TOKEN_HEREDOC,   // <<
    TOKEN_EOF        // End of input
} t_token_type;

typedef struct s_token {
    t_token_type    type;
    char           *text;
    bool           space_before;
    struct s_token *next;
} t_token;
```

### Command Structure (`include/parser.h`)
```c
typedef struct s_cmd {
    char           **args;        // Command arguments
    t_redirection  *redirections; // Input/output redirections
    struct s_cmd   *next;         // Next command in pipeline
} t_cmd;

typedef struct s_redirection {
    t_redir_type         type;     // <, >, >>, <<
    char                *file;     // Target file/delimiter
    int                  fd;       // File descriptor
    struct s_redirection *next;    // Next redirection
} t_redirection;
```

### Fragment Structure (`include/lexer.h`)
```c
typedef enum {
    QUOTE_NONE,
    QUOTE_SINGLE,
    QUOTE_DOUBLE
} t_quote_type;

typedef struct s_fragment {
    char              *text;
    t_quote_type       quote_type;
    struct s_fragment *next;
} t_fragment;
```

---

## Execution Pipeline

### Phase 1: Lexical Analysis
**Input**: Raw command string
**Output**: List of tokens
**Process**:
1. **Fragment Parsing**: Break input into quote-aware fragments
2. **Token Classification**: Identify words, operators, redirections
3. **Quote Processing**: Handle single/double quotes and escapes
4. **Validation**: Ensure syntactic correctness

**Example**:
```
Input:  echo "hello world" | grep hello
Tokens: [WORD:echo] [WORD:"hello world"] [PIPE:|] [WORD:grep] [WORD:hello]
```

### Phase 2: Parsing
**Input**: Token list
**Output**: Command structures
**Process**:
1. **Command Grouping**: Group tokens into commands
2. **Argument Assembly**: Build argument arrays
3. **Redirection Parsing**: Extract I/O redirections
4. **Pipeline Construction**: Link commands with pipes

**Example**:
```
Tokens: [echo] ["hello world"] [|] [grep] [hello]
Commands: 
  - cmd1: {args: ["echo", "hello world"], redirs: [], next: cmd2}
  - cmd2: {args: ["grep", "hello"], redirs: [], next: NULL}
```

### Phase 3: Expansion
**Input**: Parsed commands
**Output**: Expanded commands
**Process**:
1. **Variable Expansion**: Replace `$VAR` with values
2. **Quote Removal**: Remove outer quotes, preserve inner
3. **Tilde Expansion**: Convert `~` to home directory
4. **Dollar Quote Processing**: Handle `$"string"` syntax

**Example**:
```
Before: echo "$USER's home: ~"
After:  echo "john's home: /home/john"
```

### Phase 4: Execution
**Input**: Expanded commands
**Output**: Program execution
**Process**:
1. **Built-in Check**: Route to built-in functions if applicable
2. **Process Creation**: Fork for external commands
3. **Pipeline Setup**: Configure pipes between commands
4. **Redirection**: Set up file descriptors
5. **Execution**: Execute via execve() or built-in handler

---

## Error Handling

### Error Categories

#### 1. Syntax Errors (Parser)
- Unclosed quotes: `echo "unclosed`
- Invalid redirections: `echo > > file`
- Pipeline errors: `| echo hello`

#### 2. Runtime Errors (Executor)
- Command not found: `nonexistent_command`
- Permission denied: `./no_exec_permission`
- File not found: `cat nonexistent.txt`

#### 3. System Errors
- Memory allocation failures
- File descriptor exhaustion
- Fork failures

### Error Reporting Strategy
```c
// Consistent error format
ft_putstr_fd("minishell: ", STDERR_FILENO);
ft_putstr_fd(command, STDERR_FILENO);
ft_putstr_fd(": ", STDERR_FILENO);
ft_putendl_fd(error_message, STDERR_FILENO);
```

---

## Memory Management

### Memory Allocation Strategy

#### 1. Token Management
```c
// Allocation
t_token *token = malloc(sizeof(t_token));
token->text = ft_strdup(input_text);

// Cleanup
void free_token_list(t_token *tokens);
```

#### 2. Command Management
```c
// Allocation
t_cmd *cmd = malloc(sizeof(t_cmd));
cmd->args = malloc(sizeof(char*) * (argc + 1));

// Cleanup
void free_cmd_list(t_cmd *cmds);
```

#### 3. Environment Management
```c
// Copy environment for modifications
char **envp_copy = new_envp(envp);

// Cleanup
void free_envp(char **envp);
```

### Memory Safety Principles
1. **Every malloc has a corresponding free**
2. **Null pointer checks before access**
3. **Clean error handling with proper cleanup**
4. **No memory leaks in normal operation**

---

## Testing Framework

### Test Categories

#### 1. Unit Tests
- Individual function testing
- Module isolation testing
- Edge case validation

#### 2. Integration Tests
- Cross-module interaction
- Pipeline functionality
- Error propagation

#### 3. Regression Tests
- Ensure fixes don't break existing functionality
- Compatibility with bash behavior
- Performance validation

### Testing Tools
```bash
# Comprehensive test suite
./42_minishell_tester/tester.sh m

# Quote and variable specific tests
./test_quote_variable_baseline.sh

# Detailed debugging
./debug_detailed.sh

# Progress tracking
./progress_tracker.sh
```

---

## Module Interfaces

### Lexer Interface (`include/lexer.h`)
```c
// Primary functions
t_token *tokenize_input(const char *input);
t_fragment *parse_fragments(const char *text);
void free_token_list(t_token *tokens);

// Token utilities
t_token *create_token(t_token_type type, bool space_before);
void append_token(t_token **list, t_token *new_token);
```

### Parser Interface (`include/parser.h`)
```c
// Primary functions
t_cmd *parser_tokens(t_token *tokens);
void free_cmd_list(t_cmd *cmds);

// Command utilities
t_cmd *create_cmd(void);
void add_redirection(t_cmd *cmd, t_redirection *redir);
```

### Executor Interface (`include/executor.h`)
```c
// Primary functions
int execute_commands(t_cmd *cmds, char **envp, t_exec_state *state);
int expand_cmd_inplace(t_cmd *cmd, char **envp, t_exec_state *state);

// Expansion utilities
char *expand_variables(const char *input, char **envp, t_exec_state *state);
char *remove_quotes(const char *input);
```

### Builtins Interface (`include/builtins.h`)
```c
// Built-in detection and execution
int is_builtin(const char *command);
int execute_builtin(t_cmd *cmd, char **envp, t_exec_state *state);

// Individual built-ins
int builtin_echo(char **args);
int builtin_cd(char **args, char **envp);
int builtin_export(char **args, char ***envp);
```

---

## Configuration and Constants

### Compile-time Configuration (`include/minishell.h`)
```c
#define PROMPT "minishell$ "
#define MAX_ARGS 1024
#define MAX_ENV_VARS 1024
#define BUFFER_SIZE 4096
```

### Runtime State Management
```c
typedef struct s_exec_state {
    int last_status;    // Exit status of last command
    int pipe_fd[2];     // Pipe file descriptors
    pid_t *pids;        // Process IDs for cleanup
    int pid_count;      // Number of processes
} t_exec_state;
```

---

## Extension Points

### Adding New Built-ins
1. **Add function prototype** to `include/builtins.h`
2. **Implement function** in appropriate `src/builtins/` file
3. **Register in dispatcher** (`src/builtins/builtins.c`)
4. **Add tests** for new functionality

### Adding New Token Types
1. **Update enum** in `include/lexer.h`
2. **Add recognition logic** in lexer
3. **Update parser** to handle new type
4. **Add execution logic** if needed

### Adding New Expansions
1. **Add expansion function** to `src/executor/expand_*.c`
2. **Integrate in expansion pipeline**
3. **Add tests** for new expansion type

---

This architecture provides a solid foundation for a robust shell implementation with clear separation of concerns, proper error handling, and extensible design patterns.
