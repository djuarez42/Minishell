## Minishell — Project structure, functions, and expected bash behavior

Date: 2025-08-17

### Overview

This document explains the repository layout, core data structures, the purpose of key functions in each module, and the bash-compatible behavior targeted by this minishell. It also documents environment and variable expansion rules used throughout.

## Directory layout

- `include/`: Public headers for modules (lexer, parser, executor, builtins, minishell).
- `src/lexer/`: Tokenization helpers and utilities.
- `src/parser/`: Transforms tokens into executable command structures.
- `src/executor/`: Expands variables, handles redirections/pipelines, runs builtins or external commands.
- `src/builtins/`: Builtin commands implementations.
- `libft/`: Local C library (string/ctype/memory helpers).
- `main.c`: REPL (read–eval–print loop) and program entry point.
- `tests/`: Bash-vs-minishell comparison scripts.

## Data structures (variables and structure)

- `t_token` (lexer output)
  - Fields: `type` (WORD, PIPE, REDIRECT_IN, REDIRECT_OUT, APPEND, HEREDOC, EOF), `value`, `next`.
  - Represents raw lexical units with minimal normalization.

- `t_redir` (redirections)
  - Fields: `type` (in/out/append/heredoc), `file` (target), `next`.
  - Created by parser when it sees `<`, `>`, `>>`, `<<`.

- `t_cmd` (single command node)
  - Fields: `argv` (NULL-terminated), `redirs` (`t_redir*`), `pipe` (0/1 flag marking pipe continuation), `next` (next command in list).
  - One `t_cmd` per command segment; consecutive nodes chained for pipelines.

- `t_exec_state`
  - Fields: `last_status` (int).
  - Tracks `$?` expansion and last exit status updates across commands.

- Environment representation
  - A `char **envp_copy` is maintained by minishell (copied from the host environment on startup).
  - Helpers in `env_utils.c` manage lookups, insertion/removal, and controlled ordering.

## Program flow (high-level)

1) Read input:
   - Interactive: `readline("minishell$ ")`.
   - Non-interactive: `read_stdin_line()` reads one line per iteration until EOF.
2) Lexing: `tokenize_input()` → `t_token` list.
3) Parsing: `parser_tokens()` → linked list of `t_cmd`.
4) Expansion: `expand_cmd_inplace()` performs `$` expansions in argv and redirection targets.
5) Execution: `executor()`
   - Single builtin without a pipe → run in parent (`run_builtin_in_parent`).
   - Pipelines or external → fork/exec in `run_pipeline`.
6) Update `last_status`, free structures, loop.

## Module reference and key functions

### main.c
- `read_stdin_line()`: Non-interactive line reader; returns one logical line per loop.
- `main()`: Initializes `envp_copy`, drives the REPL loop, invokes lexer, parser, expansion, and executor.

### Lexer (src/lexer)
- `tokenize_input(const char *input)`:
  - Calls `clean_input_quotes()` to strip shell comments (`#` outside quotes) and reconstruct tokens.
  - Builds a `t_token` list via `build_token_list()` and appends `TOKEN_EOF`.
- `clean_input_quotes(const char *input)`:
  - `strip_comments()`: Removes `# ...` unless inside quotes.
  - `reconstruct_words()`: Walks the line, splitting into tokens while respecting quotes and spaces.
  - Helpers: `handle_quoted_part()`, `handle_plain_text()`, `process_spaces_and_quotes()`.
- `extract_quoted_segment()`: Extracts quoted content; used to assemble tokens without surrounding quotes.
- `remove_quotes(char *str)`: Simple quote stripping where needed for redirection file names.
- `determine_token_type(char *str)`: Classifies standalone operators (`|`, `<`, `>`, `<<`, `>>`) vs words.

Expected bash-aligned behavior:
- `#` starts a comment when unquoted; entire remainder of line is ignored.
- Quotes group text; no attempt to perform C-style backslash escapes here (kept minimal per project scope).

### Parser (src/parser)
- `parser_tokens(t_token *tokens)`: Creates a list of `t_cmd` nodes.
- `create_cmd_node()`: Parses one command segment (argv first, then redirections) and marks `pipe` if `|` follows.
- `parse_arguments()`: Fills `argv` with `TOKEN_WORD` values for the command.
- `parse_redirections()`: Builds a linked list of `t_redir` from redirect tokens plus their `TOKEN_WORD` targets.
- `free_cmds()`, `free_redirs()`: Cleanup helpers.

### Variable expansion (src/executor/expand_*.c)
- `expand_cmd_inplace(t_cmd *cmd, char **envp, t_exec_state *state)`:
  - Calls `expand_argv()` and `expand_redirs()`.
- `expand_variables(const char *input, char **envp, t_exec_state *state)`:
  - Walks the string and splices expanded pieces.
  - `$?` handled via `expand_exit_status()`; `$NAME` via `expand_env_var()`.
- `handle_dollar()`, `extract_plain_text()`: Piecewise assembly of expanded strings.

Expected behavior:
- `$VAR` → value or empty if unset.
- `$?` → last exit status.
- Inside single quotes: no expansion; inside double quotes: expand `$`.
- No word-splitting/globbing after expansion; result stays a single argv element.

### Environment helpers (src/executor/env_utils.c)
- `env_identifier_valid(const char *name)`: Validates identifier format.
- `env_find_index(char **envp, const char *name)`, `env_get_value(char **envp, const char *name)`.
- `env_set_var(char ***penvp, const char *name, const char *value)`:
  - Replaces existing, or inserts new entries at the head (affects display order with `env`).
- `env_set_assignment(char ***penvp, const char *assignment)`:
  - `NAME=VAL` form supported; name-only returns success without creating `NAME=`.
- `env_unset_var(char ***penvp, const char *name)`: Removes if present.
- `new_envp(char **envp)`, `free_envp(char **envp)`: Copy and free environment arrays.

### Builtins (src/builtins/builtins.c)
- `is_builtin(const char *cmd)`: Recognizes builtin names.
- `bi_echo(char **argv)`:
  - Supports `-n` and `-nnn...` clusters; prints arguments with spaces; newline omitted if `-n`.
- `bi_pwd(void)`: Prints current working directory; errors on `getcwd` failure.
- `bi_env(char **envp)`: Prints all `NAME=VALUE` entries.
- `bi_cd(char **argv, char ***penvp)`:
  - `cd <path>` changes directory and updates `PWD`/`OLDPWD`.
  - `cd` with no args: `HOME` used if set; otherwise (in current tests) returns success without error.
- `bi_export(char **argv, char ***penvp)`:
  - `NAME=VALUE` updates env; name-only validates but does not create `NAME=` entry.
  - Invalid identifiers print an error to stderr and set non-zero status.
- `bi_unset(char **argv, char ***penvp)`:
  - Removes valid names; silently ignores invalid identifiers.
- `bi_exit(char **argv)`: Returns exit status (caller decides to exit for parent case).
- `run_builtin_in_parent/child(...)`: Context-aware builtin execution.

Expected behavior vs bash (as used in tests):
- echo: `-n`/`-nnn` recognized; quoting preserved in tokenization; no extra spaces/newlines.
- export: name-only does not add `NAME=` to `env` output; order of newly added variables appears before older ones when printing `env`.
- unset: invalid names ignored silently.
- cd: updates `PWD`/`OLDPWD`; no-arg uses `HOME` when set (no error message when unset per current tests).
- env: prints exported variables (`NAME=VALUE`) one per line.

### Executor (src/executor)
- `executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)`:
  - Iterates command nodes; runs a single builtin in parent; otherwise builds a pipeline.
  - Updates `state->last_status` after each command/pipeline.
- `run_pipeline(...)`: Creates pipes, forks children, wires stdin/stdout, handles redirections, and executes builtins (in child) or external commands via `execute_command()`.
- `find_executable(char *cmd)`: Resolves `cmd` using `PATH` (from the host env in current implementation) unless `cmd` contains a `/`.
- `execute_command(char *exec_path, t_cmd *cmd, char **envp)`:
  - On failure to resolve, prints `bash: line 1: <cmd>: command not found` and exits with 127.

Expected behavior vs bash:
- “command not found” message and exit code 127.
- Pipelines: left-to-right execution with correct redirections.
- Builtins in pipelines run in children; a lone builtin runs in parent.

## Portability notes

- Linking (Makefile):
  - macOS: `-lreadline` (Homebrew provides libs/headers on standard paths on many setups).
  - Linux: `-lreadline -lncurses -ltinfo` to satisfy transitive dependencies on some distros.
- PATH search uses the host env (`getenv("PATH")`) for external commands. Environment changes within minishell do not currently affect PATH search for `execve`.

## Test-aligned behaviors (quick reference)

- `env_print`: `#` handled as comment start (unquoted) → pass.
- `cd_and_pwd`: PWD/OLDPWD updated when running separate lines → pass.
- `missing_cmd`: stderr: `bash: line 1: <cmd>: command not found` → pass.
- `echo_n`: multi-line non-interactive processing; `echo -n` followed by `printf` on next line prints `no-newline<END>` → pass.
- `cd_missing`: `cd` with no args uses `HOME` if set; no custom error on unset (per tests) → pass.
- `export_unset`: order parity via head insertion; `unset` removes var → pass.
- `export_name_only`: no `NAME=` created for name-only → pass.
- `unset_invalid`: invalid identifiers ignored silently → pass.


