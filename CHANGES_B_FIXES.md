## Contributor B Fixes — What changed and why

Date: 2025-08-17

### Summary

This document tracks the parser/executor side fixes made for failing tests in the Contributor B suite.

### Fixed

- Missing command error format (test: `missing_cmd`)
  - File: `src/executor/executor_utils.c`
  - Change: Updated not-found error to exactly `bash: line 1: <cmd>: command not found` on stderr, exit 127.
  - Reason: Tests compare exact stderr content against bash.

- `cd` with no arguments (test: `cd_missing`)
  - File: `src/builtins/builtins.c`
  - Change: If no argument, `cd $HOME` if set; if `HOME` is unset, return success without printing an error. Always update `PWD`/`OLDPWD` after a successful `chdir`.
  - Reason: Align with bash behavior; remove custom `minishell: cd: missing operand` line.

### Input reading improvements (non-interactive)

- Implemented strict line-by-line reader for `!isatty(STDIN_FILENO)`
  - File: `main.c`
  - Change: Added `read_stdin_line()` which reads one byte at a time until `\n` and returns one line per loop iteration; integrated into the main loop when stdin is not a TTY.
  - Reason: Ensure each newline in redirected scripts yields a separate REPL iteration.

### Fixed: `echo_n` sequencing

- Files: `main.c`, `src/lexer/tokenizer_utils_2.c`, `src/lexer/lexer_utils_4.c`
- Changes:
  - Non-interactive strict line reader ensures each line is processed separately.
  - Token type detection tightened to treat only standalone operators as operators.
  - Quoted segment handling preserves content for tokens like `"<END>\n"`.
- Result: `echo_n` now prints `no-newline<END>` and test passes.

### Test results after fixes

- Contributor B: PASS for `env_print`, `cd_and_pwd`, `missing_cmd`, `cd_missing`, `echo_n`.

### Files touched

- `src/executor/executor_utils.c`
- `src/builtins/builtins.c`
- `main.c` (added non-interactive fallback reader)


