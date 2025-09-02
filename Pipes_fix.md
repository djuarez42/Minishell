# Pipe Implementation Analysis in Minishell

## 1. Syntax Validation (Parser Stage)

Your shell properly validates pipe syntax through the `validate_pipe_syntax` function in `parser_errors.c`. It correctly checks for three invalid pipe scenarios:

### Leading Pipe: `|ls | ls`

- **Bash Expected Behavior**: Syntax error
- **Minishell Implementation**: 
  - Detected in the check: `if (tokens->type == TOKEN_PIPE)`
  - Error message: "syntax error near unexpected token `|'"

### Consecutive Pipes: `ls || ls`

- **Bash Expected Behavior**: Syntax error (unless in conditional context like `if [ ] || [ ]`)
- **Minishell Implementation**:
  - Detected in the check: `if (cur->type == TOKEN_PIPE && cur->next->type == TOKEN_PIPE)`
  - Same error message

### Trailing Pipe: `ls |`

- **Bash Expected Behavior**: Syntax error or prompt for continuation
- **Minishell Implementation**:
  - Detected in the check: `if (cur && cur->type == TOKEN_PIPE)`
  - Same error message

## 2. Command Parsing and Pipeline Structure

During command parsing:

- Each command is assigned a `pipe` flag (0 or 1) in the `t_cmd` structure
- The `pipe` flag is set to 1 if the command is followed by a pipe
- Commands are linked in a chain, and each pipe creates a segment in this chain

## 3. Pipeline Execution (Executor Stage)

The pipeline execution follows these steps:

### Pipeline Detection: `count_pipeline_cmds` counts how many commands are in a pipeline

```c
while (start) {
    count++;
    if (start->pipe == 0)
        break;
    start = start->next;
}
```

### Pipe Creation: `create_pipes` allocates and initializes N-1 pipes for N commands

```c
*pipes = (int (*)[2])malloc(sizeof(int[2]) * n_pipes);
// ...
pipe((*pipes)[i])
```

### Process Creation: For each command in the pipeline:
- Fork a child process
- Set up signal handling in the child
- Wire up the pipes

### Pipe Wiring: `wire_child_pipes` connects stdin/stdout to appropriate pipe ends

```c
// Connect previous command's output to this command's input
if (idx > 0) {
    dup2(pipes[idx - 1][0], STDIN_FILENO);
}

// Connect this command's output to next command's input
if (idx < n_cmds - 1) {
    dup2(pipes[idx][1], STDOUT_FILENO);
}
```

### Cleanup: Close all pipe descriptors in both parent and children

### Wait for Completion: `wait_pipeline` waits for all child processes, returning the exit status of the last command

## 4. Special Case Handling

Your implementation has special handling for commands in a pipeline vs. standalone commands:

### Standalone Builtins: Run directly in the parent process

```c
if (n == 1 && is_builtin(cur->argv[0]) && cur->pipe == 0)
```

### Piped Builtins: Run in a child process to ensure output flows through the pipe

```c
if (is_builtin(cur->argv[0]))
    exit(run_builtin_in_child(cur, &envp));
```

### External Commands: Always executed in a child process using `execve`

## Examples Analysis with Bash Expected Behavior

Let's analyze how your shell would handle each example compared to Bash:

### `ls | cat asd`

- **Bash Expected Behavior**:
  - Creates a pipeline with 2 commands
  - `ls` output is piped to `cat`
  - `cat` reports "No such file or directory" for "asd" but still processes stdin
  - Output: Directory listing appears with an error for the nonexistent file

- **Minishell Implementation**:
  - Creates a pipeline with 2 commands
  - `ls` output is sent to `cat`'s stdin
  - `cat` tries to read "asd" but falls back to stdin if file not found
  - Output: Directory listing appears (possibly with "No such file" error)

### `ls | cat << stop | grep "asd"`

- **Bash Expected Behavior**:
  - Creates a pipeline with 3 commands
  - `ls` output is ignored because `cat` reads from the here-document
  - Here-document content is sent to `grep`
  - If "asd" is not in the here-document, no output is produced

- **Minishell Implementation**:
  - Creates a pipeline with 3 commands
  - Here-document for `cat` is handled in the executor
  - The here-document content replaces stdin from `ls`
  - `grep` filters for "asd" (likely no output since heredoc doesn't contain "asd")

### `ls | ls | ls | ls | ls | ls`

- **Bash Expected Behavior**:
  - Creates a pipeline with 6 commands
  - Each command runs in its own process
  - Each `ls` ignores its stdin and produces a listing of the current directory
  - Only the output of the last `ls` is displayed
  - Output: Simple directory listing from last `ls`

- **Minishell Implementation**:
  - Creates a pipeline with 6 commands
  - Each `ls` discards input from previous command
  - Only the last `ls` output is displayed
  - Output: Simple directory listing from last `ls`

### `|ls | ls`

- **Bash Expected Behavior**:
  - Syntax error: "syntax error near unexpected token `|'"
  - No command execution

- **Minishell Implementation**:
  - Triggers syntax error in `validate_pipe_syntax`
  - Error: "syntax error near unexpected token `|'"

### `ls|cat Makefile|ls`

- **Bash Expected Behavior**:
  - Functionally identical to `ls | cat Makefile | ls`
  - `ls` output is piped to `cat`
  - `cat` reads Makefile and sends both `ls` output and Makefile content to the second `ls`
  - Second `ls` ignores its stdin and shows directory listing
  - Output: Directory listing from last `ls`

- **Minishell Implementation**:
  - Functionally identical to `ls | cat Makefile | ls`
  - Creates a pipeline with 3 commands
  - `cat` reads Makefile and sends to `ls`
  - `ls` discards its input and shows directory listing
  - Output: Directory listing from last `ls`
