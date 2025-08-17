## Norminette Report — Minishell

Date: 2025-08-17

This report summarizes current Norminette violations and proposes concrete remediation steps per file. Run with: `norminette`.

### Summary

- Key themes to fix:
  - Tabs vs spaces, indentation, long lines, and scope comments
  - Functions too long (>25 lines) / too many functions per file
  - Return value parenthesis, operators at EOL, consecutive blank lines

### Violations by file and proposed actions

- main.c
  - Errors: too many variables in function, wrong scope comments, function > 25 lines
  - Actions:
    - Extract helpers: input reading (`read_stdin_line` already exists), tokenizing/executing loop into `process_input_line`, history handling helper
    - Remove inline comments or move above code; ensure max 25 lines per function

- include/builtins.h
  - Errors: consecutive spaces, spaces instead of tabs
  - Actions: convert indentation to tabs, remove extra spaces, align prototypes

- include/executor.h
  - Errors: consecutive newlines
  - Actions: collapse extra blank lines between declarations

- include/mini_libft.h
  - Errors: missing/invalid 42 header, bad preprocessor indent, spaces vs tabs
  - Actions: add 42 header, indent `#ifndef/#define/#endif` with tabs per norm, fix spaces to tabs, align typedefs/prototypes

- libft/ft_substr.c
  - Errors: empty line at EOF
  - Actions: remove trailing blank line at file end

- src/lexer/tokenizer.c
  - Errors: expected newline after brace
  - Actions: ensure opening/closing braces have line breaks per norm

- src/lexer/lexer_utils_4.c
  - Errors: missing tabs for indent level, spaces instead of tabs, long lines, wrong scope comments, empty lines in functions, too many lines, too many functions
  - Actions: convert spaces to tabs, split helpers into separate file(s) if needed, wrap long lines (<80), remove inline comments inside blocks, keep functions <= 25 lines

- src/builtins/builtins.c
  - Errors: spaces before function name, tabs/indent issues, operators at EOL, too many lines/functions, return parenthesis, many spacing issues
  - Actions: convert indentation to tabs, wrap long expressions, avoid operators at end of line, split into files or static helpers (echo flags, cd path resolve, export arg handling, unset loop)

- src/executor/executor.c
  - Errors: spacing after keywords, missing space before parentheses, long functions, too many funcs, long lines, ternary forbidden, scope comments
  - Actions: replace ternaries with if/else, split `run_pipeline`, `executor` into smaller helpers, fix spacing around keywords and parentheses

- src/executor/expand_variables.c
  - Errors: function > 25 lines
  - Actions: split `expand_variables` into smaller helpers (handle_dollar piece is already in utils; move plain text extraction fully to utils and keep the main loop minimal)

- src/executor/env_utils.c
  - Errors: too many functions in file, misaligned var decl, wrong scope comments, long lines, return parenthesis
  - Actions: move identifier/lookup helpers to a separate `env_lookup.c`, align declarations, wrap long lines, enforce `(return value)` format where required by norm

### Remediation plan

1) Formatting sweep (low-risk, high-yield)
   - Replace spaces with tabs where norm expects tabs in headers and sources
   - Remove extra blank lines and inline comments flagged as wrong scope
   - Wrap lines to <80 chars

2) Function size/structure (moderate effort)
   - Split oversized functions into cohesive helpers: `main`, `executor.c`, `builtins.c`, `lexer_utils_4.c`, `env_utils.c`, `expand_variables.c`
   - Move groups of helpers into new compilation units to reduce "too many funcs in file"

3) Semantic style fixes
   - Replace ternaries, remove operators at EOL, ensure `return (value);`
   - Ensure braces and keyword spacing follow norm

### Suggested ordering (to minimize churn)

- Pass 1: Headers (include/*.h) — tabs/spacing and header guards
- Pass 2: libft trailing EOF newline fixes
- Pass 3: `main.c` and `executor.c` splits
- Pass 4: `builtins.c` splits and spacing
- Pass 5: `lexer_utils_4.c` and tokenization helpers (split/comment cleanup)
- Pass 6: `env_utils.c` and `expand_variables.c` splits

### Re-run after each pass

- Command: `norminette | sed -n '1,120p'`
- Track deltas and stop when repository reports OK


