## Scope

Design and function definitions for implementing and aligning behavior of:
- export builtin
- unset builtin
- $-variable expansion in command arguments and redirections

Targets: mandatory behavior only; match tests in `tests/contributor_a_tests.sh` and `tests/contributor_b_tests.sh`.

## Summary of expected behavior

- **export NAME=VALUE [NAME=VALUE ...]**: set or replace env vars. New variables should appear before earlier ones when listed (affects grep order in tests).
- **export NAME** (no `=`): do not create `NAME=` in the environment. Validate identifier; success if valid, no-op if absent.
- **unset NAME [NAME ...]**: remove env entries if present; ignore invalid identifiers silently (no stderr, exit 0).
- **$ expansion**: replace `$VAR` with value; unset expands to empty. `$?` expands to last exit status. Do not expand inside single quotes. Expand inside double quotes. No word-splitting or globbing.

## Files and functions to implement/update

- `src/executor/env_utils.c`
  - `int env_identifier_valid(const char *name);`
  - `int env_find_index(char **envp, const char *name);`
  - `char *env_get_value(char **envp, const char *name);`
  - `int env_set_var(char ***penvp, const char *name, const char *value);`
  - `int env_set_assignment(char ***penvp, const char *assignment);`
  - `int env_unset_var(char ***penvp, const char *name);`

- `src/builtins/builtins.c`
  - `int bi_export(char **argv, char ***penvp);`
  - `int bi_unset(char **argv, char ***penvp);`

- `src/executor/expand_variables*.c`
  - `int expand_cmd_inplace(t_cmd *cmd, char **envp, t_exec_state *state);` (already present)
  - Helpers to scan and expand `$` respecting quotes.

## Detailed semantics and realization

### env_set_var (prepend new entries)

- Signature: `int env_set_var(char ***penvp, const char *name, const char *value);`
- Behavior:
  - Validate `name` via `env_identifier_valid()`. Return 0 on invalid.
  - Build `entry = name + "=" + value` (value may be empty string; do not pass NULL as a signal for no-`=` entry).
  - If `name` exists, replace in-place and return 1.
  - If new, reallocate `envp` and insert the new pointer at index 0 (shift existing entries to the right). This yields observable ordering where the most recently exported variable appears first under `env | grep ...` in tests.

Pseudocode:
```c
idx = env_find_index(*penvp, name);
if (idx >= 0) { free((*penvp)[idx]); (*penvp)[idx] = entry; return 1; }
count = count_env(*penvp);
if (!realloc_env(penvp, count + 1)) { free(entry); return 0; }
// shift right
for (i = count; i > 0; --i) (*penvp)[i] = (*penvp)[i - 1];
(*penvp)[0] = entry;
(*penvp)[count + 1] = NULL; // ensure terminator
return 1;
```

### env_set_assignment (name-only delegation)

- Signature: `int env_set_assignment(char ***penvp, const char *assignment);`
- Behavior:
  - If `assignment` contains `=`, split `name` and `value` and call `env_set_var(penvp, name, value)`.
  - If no `=`, do not create an empty assignment here. Return success for valid identifiers, error for invalid. Leave any insertion decisions to `bi_export()`.

### bi_export (no name-only insertion)

- Signature: `int bi_export(char **argv, char ***penvp);`
- Behavior:
  - With no args: print environment (reuse `bi_env`).
  - For each arg:
    - If contains `=`, call `env_set_assignment()`; on invalid identifier, print `minishell: export: \\`ARG\\`: not a valid identifier` to stderr and set non-zero status.
    - If name-only:
      - Validate name. If invalid, print the same error and set non-zero status.
      - If valid, do not call `env_set_var()`; this is a no-op regarding the environment content (marks as exported in a full shell, but for tests we avoid adding `NAME=`). Return success for that arg.
  - Return aggregated status (0 when all valid).

Notes:
- This aligns our behavior with bash for `export ZED` where `env | grep '^ZED='` produces no output.

### bi_unset (silent on invalid)

- Signature: `int bi_unset(char **argv, char ***penvp);`
- Behavior:
  - For each arg `name`:
    - If `env_identifier_valid(name)` is false: ignore silently (do not print errors, do not change status).
    - Else call `env_unset_var(penvp, name)` and continue.
  - Return 0. Tests expect no stderr and success when unsetting invalid identifiers.

### $-variable expansion

- Entry: `int expand_cmd_inplace(t_cmd *cmd, char **envp, t_exec_state *state);`
- Behavior per token (argv elements and redirection targets after quote removal for files):
  - Parse string left-to-right maintaining `quote_state` in {none, single, double}.
  - When in none or double state, recognize `$` sequences:
    - `$?` -> replace with decimal string of `state->last_status`.
    - `$VAR` where `VAR` matches `[A-Za-z_][A-Za-z0-9_]*` -> replace with `env_get_value(envp, VAR)` or empty if unset.
    - `$$` (optional): may be left as-is unless explicitly required; not used in current tests.
  - When in single-quote state, do not expand `$`.
  - Do not perform field splitting: expansions produce a single string fragment.
  - Do not perform globbing.

Pseudocode core:
```c
for each char c in s:
  switch (quote_state_transition(c)) { /* update state and continue if quote */ }
  if ((state == NONE || state == DQUOTE) && c == '$') {
    if (next == '?') append_itoa(state->last_status); advance 2; continue;
    if (is_name_start(next)) { name = read_name(); val = env_get_value(envp, name); append(val ?: ""); continue; }
  }
  append(c);
```

## Edge cases and error handling

- `export` with invalid names: print error to stderr and set exit status non-zero, but continue processing other args.
- `unset` with invalid names: ignore silently; overall status should remain 0.
- `export A=1 B=two`: After these operations, `env | grep -E '^(A|B)='` should print `B=two` then `A=1` based on head-insert ordering.
- `$` inside single quotes: do not expand. Inside double quotes: do expand.
- Unset variables in expansion: expand to empty string.

## Test alignment

- `export_unset`: variable ordering parity ensured by head insertion in `env_set_var()`.
- `export_name_only`: no `ZED=` appears after `export ZED`.
- `unset_invalid`: no stderr, zero exit.
- `$` expansion: covered indirectly through existing command execution and redirection tests.

## Implementation checklist

- [ ] Modify `env_set_var()` to prepend new entries.
- [ ] Adjust `env_set_assignment()` to return success for name-only without adding entries.
- [ ] Update `bi_export()` name-only branch to no-op on env content for valid names.
- [ ] Update `bi_unset()` to suppress invalid identifier errors and return 0.
- [ ] Verify `$` expansion logic respects quotes and `$?`.
- [ ] Run `make test-a` and `make test-b`; ensure targeted tests pass.


