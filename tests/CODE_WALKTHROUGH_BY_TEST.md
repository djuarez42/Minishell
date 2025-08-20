### Minishell — Code Walkthrough by Test Area (Theory + Implementation)

This document explains, step by step, how the codebase implements each area covered by the comprehensive tests. It maps theory to concrete files and functions so you can navigate the code quickly.

Contents
- A. End‑to‑end flow
- B. PATH & Executable search
- C. Navigation (pwd/cd) and directories
- D. Environment variables (env/export/unset)
- E. Quotes and escaping
- F. Pipes
- G. Redirections (>, >>, <, <<)
- H. Builtins (echo, cd, pwd, export, unset, env, exit)
- I. Environment expansion and $?
- J. Signals (Ctrl‑C/\, Ctrl‑D)
- K. Scripts and path resolution
- L. Return values and status propagation
- M. History
- N. Error handling and messages
- O. Global variables and signals rule

---

## A) End‑to‑end flow

Theory
- A shell reads one line, lexes it into tokens, parses into command structures (argv + redirections + pipe links), expands variables, then executes builtins or external commands. Repeat until EOF.

Implementation
- `main.c`
  - Prompt/input: `readline(PROMPT)` when TTY, or `read_stdin_line()` for non‑interactive.
  - Tokenization: `tokenize_input(input)` → `t_token *`.
  - Parsing: `parser_tokens(tokens)` → linked list `t_cmd *` (one node per command, linked by `pipe`).
  - Expansion: `expand_cmd_inplace(cur, envp_copy, &state)` per command.
  - Execution: `executor(cmds, &envp_copy, &state)`; free all allocations, loop.

Key data
- `t_cmd`: argv array, redirection list, `pipe` flag, `argv_quote` metadata.
- `t_exec_state`: holds `last_status` (shell’s `$?`).

---

## B) PATH & Executable search

Theory
- External command resolution: if the command contains `/`, use as given. Otherwise, search `PATH` directories for an `X_OK` executable.

Implementation
- `src/executor/executor_utils.c` → `execute_command`
  - Uses `find_executable` to resolve path.
- `src/executor/executor.c` → `find_executable`
  - Splits `PATH` (`env_get_value(envp, "PATH")`, `ft_split(':')`) and checks `access(full_path, X_OK)`.
- Errors
  - On failure: prints `bash: line 1: <cmd>: command not found` (stderr) and `exit(127)` in child.

Tests
- `tests/subject_full_tests.sh`: `path_basic`, `path_unset_then_restore`.

---

## C) Navigation (pwd/cd) and directories

Theory
- `pwd`: print current directory (from `getcwd`).
- `cd`: change directory; update `PWD` and `OLDPWD`. `cd` with no args → `HOME` when set.

Implementation
- `src/builtins/builtins.c`
  - `bi_pwd()`: `getcwd`, prints to stdout.
  - `bi_cd(argv, &envp)`: handles no‑arg → `HOME`, errors via `perror("cd")`.
  - `update_pwd_vars`: sets `OLDPWD` and `PWD` in env after `chdir`.

Tests
- `pwd_and_cd` in subject suite; contributor B `cd_and_pwd`, `cd_missing`.

---

## D) Environment variables (env/export/unset)

Theory
- `env`: print environment (`KEY=VALUE` lines).
- `export`: set or mark as exported; name‑only export should not insert `KEY=` into `env`.
- `unset`: remove from environment; ignore invalid names silently for tests.

Implementation
- `src/builtins/builtins.c`
  - `bi_env`, `bi_export`, `bi_unset` implement behavior and validation.
- `src/executor/env_utils.c` (not listed above, but present in project) provides helpers: `env_set_var`, `env_set_assignment`, `env_unset_var`, `env_get_value`.

Tests
- `export_unset_env` in subject suite; contributor A `export_unset`, `export_name_only`, `unset_invalid`.

---

## E) Quotes and escaping

Theory
- Single quotes `'...'` are literal: no expansion, no special interpretation.
- Double quotes "..." allow `$VAR`/`$?` expansion but keep spaces together as one argument.
- The subject says: do not interpret special characters not required (like backslash `\` or semicolon `;`). They should be treated as regular characters if not otherwise implemented.

Implementation
- Lexer: `src/lexer/*.c`, especially `tokenizer.c`, `lexer_utils_*.c`
  - Tracks quote state; splits tokens on spaces unless inside quotes.
  - Stores per‑token quote type in `argv_quote` to control later expansion.
- Expansion: `src/executor/expand_variables*.c`
  - `expand_argv(argv, argv_quote, envp, state)`: expands only when not single‑quoted.

Tests
- `single_quotes_literal`, `double_quotes_expand` in subject suite; quoting tests in smoke.

---

## F) Pipes

Theory
- `cmd1 | cmd2 | cmd3`: connect stdout of each to stdin of the next via `pipe()` and `dup2()`.
- Parent forks children, wires fds, closes all unused pipe ends, waits; last child’s exit status becomes `$?`.

Implementation
- `src/executor/executor.c`
  - `count_pipeline_cmds`, `create_pipes`, `wire_child_pipes`, `wait_pipeline`.
  - In each child: set `SIGINT`/`SIGQUIT` to defaults; wire fds; apply redirections; builtin (in child) or `execve`.
- Single builtin without pipes is executed in parent for state/environment continuity.

Tests
- `pipes_simple`, `pipes_chain` in subject suite; contributor tests for pipelines and echo in pipeline.

---

## G) Redirections (>, >>, <, <<)

Theory
- `>` overwrite, `>>` append, `<` input from file, `<<` heredoc (read until delimiter). Quoted heredoc delimiter disables expansion; unquoted enables `$` expansion.

Implementation
- `src/executor/redir_utils.c`
  - Out/in/append with `open`, `dup2`.
  - `handle_redirections_heredoc(...)` orchestrates heredoc temp file + redirect stdin.
- `src/executor/redir_utils2.c`
  - `open_heredoc_file()`, `write_heredoc_lines(args)`, `redirect_stdin_heredoc()`.
  - On `readline("> ") == NULL` (Ctrl‑C), return 130 to cancel execution.
- Return‑code propagation up to executor ensures heredoc cancelation skips command.

Tests
- Subject: `redir_out_in_append`, `redir_in`, heredoc quoted/unquoted expansion.

---

## H) Builtins (echo, cd, pwd, export, unset, env, exit)

Theory & Implementation
- `echo` with `-n`/`-nnn...`: `bi_echo` implements `is_n_flag` and prints without trailing `\n` when flagged.
- `cd`, `pwd`, `export`, `unset`, `env`, `exit`: see sections C and D for details; `bi_exit` returns status to caller; parent path exits if builtin is `exit`.

Files
- `src/builtins/builtins.c` contains all builtins and dispatchers `run_builtin_in_parent/child`.

---

## I) Environment expansion and $?

Theory
- `$VAR` expands to `env[VAR]`; `$?` expands to last foreground pipeline’s exit code.
- Inside single quotes: no expansion; inside double quotes and bare tokens: expand.

Implementation
- `src/executor/expand_variables.c` and `expand_variables_utils*.c`
  - `handle_special_dollar` handles `$?` and `$$` special cases.
  - `handle_dollar` extracts var name and calls `expand_env_var`.
  - `expand_exit_status(&state)` returns string for `$?`.
  - `expand_cmd_inplace` applies to `argv` and redirections.

Tests
- Subject: `expand_env_and_status`.

---

## J) Signals (Ctrl‑C/\, Ctrl‑D)

Theory
- Interactive parent:
  - Ctrl‑C: newline + fresh prompt; do not exit.
  - Ctrl‑\: does nothing.
  - Ctrl‑D at empty prompt: exit (print `exit`).
- Children: default signals so programs behave normally. If a child is terminated by a signal, `$? = 128 + signum`.
- Heredoc: Ctrl‑C cancels heredoc and sets status 130.

Implementation
- `include/signals.h`, `src/signals/signals.c`
  - Set up SIGINT handler (newline + `rl_on_new_line`, `rl_redisplay`), ignore SIGQUIT, disable ECHOCTL so control characters aren’t echoed.
  - Teardown restores termios.
- `main.c` integrates setup/teardown and prints `exit` on interactive EOF.
- `executor.c` sets child signals to `SIG_DFL`.

Tests
- See `tests/SIGNALS_TESTS.txt` manual steps.

---

## K) Scripts and path resolution

Theory
- Executables run via absolute, relative, and complex relative paths; PATH search when no `/` in cmd.

Implementation
- See `find_executable` and `execute_command` (section B). Relative paths pass through to `execve` when `access` indicates existing and executable, or when the command contains `/`.

---

## L) Return values and status propagation

Theory
- `$?` should reflect: last builtin return (parent single builtin), last pipeline’s last process status, or `128+signal` when signaled.

Implementation
- `wait_pipeline` computes status; `executor` writes to `state.last_status`.
- Builtins in parent return status and parent sets `$?`.
- Expansion uses `state` for `$?`.

---

## M) History

Theory
- Keep command history in interactive mode: non‑empty lines are added; interrupted partial input should not be added.

Implementation
- `main.c`: when `*input` add to history with `add_history(input)`.
- SIGINT handler clears the line before redisplay; that partial line isn’t recorded.

---

## N) Error handling and messages

Theory
- Consistent stderr messages; proper exit codes; distinguish not found (127) vs exec errors.

Implementation
- `execute_command`: formats “bash: line 1: <cmd>: command not found” to stderr and `exit(127)`.
- Redirection errors use `perror` and `exit(1)` in child.
- `cd` errors via `perror("cd")`.

---

## O) Global variables and signals rule

Theory (subject rule)
- Avoid more than one global variable; only use a global for the signal number if needed; do not expose main data through globals.

Implementation
- The code uses an internal static pointer in `signals.c` to `t_exec_state` for last status update (not exposing full state globally). No other global state is required.

---

Cross‑reference
- Automated suites: `tests/smoke_tests.sh`, `tests/contributor_a_tests.sh`, `tests/contributor_b_tests.sh`, `tests/subject_full_tests.sh`.
- Manual references: `tests/SUBJECT_MANUAL_CMDS.txt`, `tests/TESTING_GUIDE_EN.md`, `tests/SIGNALS_TESTS.txt`.


