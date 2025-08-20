### Minishell 42 — Contributor Guide, Work Split, and Test Plan

This document defines guidelines, roles, workflow, and a lean test plan for this Minishell implementation. It focuses on the mandatory requirements and clean division of responsibilities to avoid conflicts.

## Scope
- Mandatory features only. Bonus operators like `;`, `&&`, `||` are not required and may be unsupported in the main shell. Test suites that rely on them are flagged accordingly.
- Bash compatibility within scope: quoting, redirections, pipes, env expansion, builtins behavior and error messages.

## Coding Guidelines
- C standard: C99 on 42 platform.
- Warnings: `-Wall -Wextra -Werror` must be clean.
- Headers: declare public APIs in `include/`, implement in `src/...`.
- Naming and clarity:
  - Prefer descriptive names: functions are verbs, variables are nouns.
  - Avoid cryptic abbreviations.
  - Keep functions focused; prefer small helpers over deep nesting.
- Error handling:
  - Check all system calls; print precise messages to the correct stream (stderr for errors).
  - Return meaningful statuses; store shell last status in `t_exec_state.last_status`.
- Comments: explain “why”, not “what”. No noise comments.
- Formatting: match existing style and indentation; do not reformat unrelated code.

## Git Workflow
- Default branch: `main`.
- Feature branches:
  - Contributor A: `feature/builtins-behavior`
  - Contributor B: `feature/parser-exec-fixes`
- Commit messages: prefix with a domain when helpful (e.g., `builtins:`, `lexer:`, `exec:`, `tests:`). Keep subject under ~72 chars.
- PRs:
  - Keep changesets small and focused.
  - Include a short summary of tested scenarios and results (copy/paste the script output).
  - Avoid mixing refactors with functional changes.

## Work Split
### Contributor A — Builtins and Environment
- Builtins: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`.
- Environment management functions: `env_set_var`, `env_set_assignment`, `env_unset_var`, `env_get_value`, ordering and printing behavior as required by tests.
- Edge cases and exact stderr/stdout compliance.

Acceptance (mandatory):
- `export_unset`, `export_name_only`, `unset_invalid` pass in `tests/contributor_a_tests.sh`.

### Contributor B — Lexer, Parser, Executor, Signals
- Lexer/tokenizer and parser correctness for words, quotes, pipes, redirections (no bonus operators).
- Executor: pipeline wiring, redirections, builtins-in-parent vs child.
- Signals (parent):
  - SIGINT: reset line, print newline, set last status 130; do not exit.
  - SIGQUIT: ignored in the shell.
  - Disable ECHOCTL in interactive mode to hide control character echoes.
- Signals (children): set SIGINT/SIGQUIT to default before `execve` (already done).
- Heredoc: support quoted/unquoted delimiter, expansion only when unquoted; cancel on Ctrl-C, clean temp, set status 130, skip command.

Acceptance (mandatory):
- `env_print`, `cd_and_pwd`, `missing_cmd`, `echo_n`, `cd_missing` pass in `tests/contributor_b_tests.sh`.

## Testing Strategy
- Primary mandatory suites (preferred for CI and reviews):
  - `make test-a` — Contributor A scope (builtins/env)
  - `make test-b` — Contributor B scope (lexer/parser/executor/signals)
- Aggregate legacy suite:
  - `make test-builtin` — includes some bonus constructs (e.g., `;`, `&&`) and can show expected failures for unimplemented bonus features.
- Smoke tests (fast, mandatory-only):
  - `make test-smoke` — runs `tests/smoke_tests.sh` with a curated set of cases avoiding bonus operators.

## Exact Outputs and Exit Codes
- Missing command: `bash: line 1: <cmd>: command not found` on stderr, exit 127.
- Builtins must write to the correct stream; avoid extra prefixes unless required.
- On Ctrl-D at empty prompt: print `exit` and terminate.

## Non-Interactive Mode
- When stdin is not a TTY, read and execute line-by-line until EOF. Do not print prompts in outputs; ensure tests filter any residual prompt if needed.

## PR Checklist
- [ ] All mandatory tests pass locally: `make test-a`, `make test-b`, `make test-smoke`.
- [ ] No new warnings; build is clean.
- [ ] No unrelated formatting churn.
- [ ] Clear commit message and summary of test outputs attached.

## Ownership Map (quick reference)
- Builtins/env: Contributor A
- Lexer/parser/executor/signal/heredoc: Contributor B
- Shared headers and interfaces: coordinate via PR comments before edits.

## Notes on Bonus
- Operators `;`, `&&`, `||` are explicitly out of scope for mandatory. The repo may contain tests referencing them; they are allowed to fail in aggregate suites but MUST NOT affect mandatory suites.


