# Mini-shell Fix Plan and Work Split

This document defines a concrete plan to address the current test failures and split the work between two contributors. It maps each failing test to specific tasks, points to likely code locations, and defines acceptance criteria.

**IMPORTANT**: This plan focuses on **mandatory functionality only**. Tests using bonus features (&&, ||, semicolon command separation) have been modified to use separate lines instead.

Last updated: 2025-08-10

## How to run tests
- Build and run all builtin tests: `make test-builtin`
- Run contributor-specific tests: `make test-a` or `make test-b`
- Alternatively, run scripts directly:
  - `bash ./tests/builtins_tests.sh`
  - `bash ./tests/contributor_a_tests.sh` 
  - `bash ./tests/contributor_b_tests.sh`

## Current test status (updated 2025-08-10)

### Contributor A Tests (Builtins): PASS=0 FAIL=3
Failing tests:
- export_unset: Variable ordering differs (A=1,B=two vs B=two,A=1) 
- export_name_only: Shows ZED= in env when bash shows nothing
- unset_invalid: Prints error when bash is silent

### Contributor B Tests (Parser/Executor): PASS=2 FAIL=3  
Tests status:
- ✅ env_print: **PASSES** (comment handling already works)
- ✅ cd_and_pwd: **PASSES** (PWD/OLDPWD updates work correctly with separate commands)
- ❌ missing_cmd: Wrong error format ("command not found: cmd" vs "bash: line 1: cmd: command not found")
- ❌ echo_n: Missing second command execution (printf not run) - **MOVED FROM A**
- ❌ cd_missing: Prints "minishell: cd: missing operand" when bash is silent - **MOVED FROM A**

### Overall Status:
**Total Remaining Issues: 6** (3 for Contributor A + 3 for Contributor B)

Note: Tests have been updated to focus on mandatory functionality only. Bonus features (&&, semicolons) removed from test cases.

---

## Part 1 — Builtins correctness and environment behavior (Contributor A)
Focus: Align builtin commands with bash-compatible behavior and output, and ensure environment variable semantics match expectations where the tests rely on them.

Target tests:
- export_unset
- export_name_only
- unset_invalid

Likely code areas:
- src/builtins/builtins.c (lines 46-68: bi_echo, lines 148-182: bi_export, lines 184-203: bi_unset, lines 125-146: bi_cd)
- src/executor/env_utils.c (lines 89-124: env_set_var, lines 148-166: env_unset_var)
- include headers defining builtin interfaces

### Task 1: export behavior alignment
**Problem**: Environment variable insertion order and export-without-value behavior

**Expected bash behavior**:
```bash
$ export A=1 B=two
$ env | grep -E '^(A|B)='  # Shows: B=two, A=1 (specific order)
$ unset A
$ env | grep -E '^(A|B)=' | sort  # Shows: B=two only
```

**Current minishell behavior**: 
- Variables appear in different order: A=1, B=two (insertion order differs)
- Same final result but order-sensitive tests fail

**What to check**:
- `env_set_var()` in env_utils.c (lines 89-124) appends variables to end
- Environment array ordering may differ from bash's internal ordering
- Need consistent ordering strategy

**Test commands**:
```bash
# Test variable order
printf 'export A=1 B=two\nenv | grep -E "^(A|B)="\n' | ./minishell
printf 'export A=1 B=two\nenv | grep -E "^(A|B)="\n' | bash

# Compare orders
diff <(printf 'export A=1 B=two\nenv | grep -E "^(A|B)="\n' | bash) \
     <(printf 'export A=1 B=two\nenv | grep -E "^(A|B)="\n' | ./minishell)
```

**Acceptance criteria**: export_unset test passes with correct variable ordering.

---

### Task 2: export name-only behavior
**Problem**: `export NAME` (without =value) adds empty variable to environment

**Expected bash behavior**:
```bash
$ unset ZED
$ export ZED        # Mark as exported but don't add to env
$ env | grep '^ZED='  # No output (grep exit code 1)
```

**Current minishell behavior**:
```bash
$ unset ZED
$ export ZED        # Adds ZED= to environment
$ env | grep '^ZED='  # Shows: ZED=
```

**What to check**:
- `bi_export()` line 171: `env_set_var(penvp, argv[i], "")`  always adds ZED=
- Bash distinguishes between shell variables and environment variables
- `export NAME` should mark as exported but not add to environment until assigned

**Test commands**:
```bash
# Test current behavior
printf 'unset ZED\nexport ZED\nenv | grep "^ZED="\n' | ./minishell
printf 'unset ZED\nexport ZED\nenv | grep "^ZED="\n' | bash

# Check exit codes
printf 'unset ZED\nexport ZED\nenv | grep "^ZED="\necho "Exit: $?"\n' | bash
```

**Acceptance criteria**: export_name_only test passes (no output from grep).

---

### Task 3: unset validation policy
**Problem**: Error messages for invalid identifiers when bash is silent

**Expected bash behavior**:
```bash
$ unset 1INVALID    # Silent (no error message)
$ echo $?           # Exit code 0
```

**Current minishell behavior**:
```bash
$ unset 1INVALID    # Prints: minishell: unset: `1INVALID': not a valid identifier
$ echo $?           # Exit code 1
```

**What to check**:
- `bi_unset()` lines 193-199 always prints errors for invalid identifiers
- `env_identifier_valid()` correctly identifies invalid names
- Bash is lenient in some contexts and silently ignores invalid names

**Test commands**:
```bash
# Compare behaviors
printf 'unset 1INVALID\necho "Done"\n' | bash 2>&1
printf 'unset 1INVALID\necho "Done"\n' | ./minishell 2>&1

# Check exit codes
bash -c 'unset 1INVALID'; echo "Bash exit: $?"
./minishell <<< 'unset 1INVALID'; echo "Minishell exit: $?"
```

**Acceptance criteria**: unset_invalid test passes (no error output).

Engineering notes:
- Ensure builtins write to the correct streams (stdout vs stderr) exactly as expected by tests.
- Avoid extra prompts or prefixes like "minishell: " unless the test expects them.
- Add small helper to detect option patterns like -n, -nnn for echo.

Definition of done for Part 1:
- make test-builtin shows all Part 1 target tests passing.
- No regressions in previously passing tests.
- Code formatted and documented inline for any behavioral choices.

---

## Part 2 — Parser/executor fixes, command parsing, and error formatting (Contributor B)
Focus: Fix command parsing issues, builtin behavior in specific contexts, and error message formatting to match bash expectations.

Target tests:
- env_print ("command not found: #") — **ALREADY PASSES** (comment handling works)
- cd_and_pwd — **ALREADY PASSES** (PWD/OLDPWD updates work with separate lines)
- missing_cmd — error message format mismatch
- echo_n — command parsing issue (moved from A)
- cd_missing — cd without arguments behavior (moved from A)

Likely code areas:
- src/lexer/tokenizer.c and related lexer files (for command parsing)
- src/executor/executor.c and src/executor/executor_utils.c (for error messages)
- src/builtins/builtins.c (for cd_missing behavior)

### Task 1: Comment handling (#)
**Problem**: Lexer treats # as a command instead of comment start

**Expected bash behavior**:
```bash
$ # Normalize by grepping a known var we set
$ export FOO=bar
$ env | grep '^FOO='
# Output: FOO=bar
```

**Current minishell behavior**: 
- Tries to execute `#` as a command
- Results in "command not found: #" error
- Does not ignore the line as a comment

**What to check**:
- Tokenizer in src/lexer/ files doesn't recognize unquoted # as comment start
- Need to implement comment detection during lexical analysis
- Must only apply to unquoted # (preserve # inside quotes)

**Test commands**:
```bash
# Test 1: Simple comment (should be ignored)
printf '# This is a comment\necho hello\n' | ./minishell

# Test 2: Comment after command (should work)
printf 'echo hello # this is ignored\n' | ./minishell

# Test 3: # inside quotes (should NOT be treated as comment)
printf 'echo "hash # symbol"\n' | ./minishell

# Test 4: The actual failing case
printf '# Normalize by grepping a known var we set\nexport FOO=bar\nenv | grep "^FOO="\n' | ./minishell
```

**Acceptance criteria**: env_print test passes (no "command not found: #" error).

---

### Task 2: PWD/OLDPWD updates - **ALREADY WORKING**
**Status**: ✅ **FIXED** - This issue was resolved by removing bonus && syntax from tests.

**Problem was**: Test used `mkdir -p sub && cd sub && pwd` which requires && operator (bonus feature)

**Current behavior**: 
- PWD/OLDPWD updates work correctly with separate commands
- `update_pwd_vars()` function in builtins.c (lines 100-123) properly:
  - Saves current PWD to OLDPWD
  - Updates PWD with getcwd() result
  - Handles environment variable updates correctly

**Evidence of working**:
```bash
$ printf 'pwd\nenv | grep PWD\nmkdir -p sub\ncd sub\npwd\nenv | grep PWD\n' | ./minishell
/Users/ekaterina/git/mini-shell
PWD=/Users/ekaterina/git/mini-shell
OLDPWD=/tmp/test_cd
/Users/ekaterina/git/mini-shell/sub
PWD=/Users/ekaterina/git/mini-shell/sub
OLDPWD=/Users/ekaterina/git/mini-shell
```

**Test commands**:
```bash
# Test PWD/OLDPWD updates (should work)
printf 'pwd\nmkdir -p testdir\ncd testdir\npwd\nenv | grep -E "^(PWD|OLDPWD)="\n' | ./minishell

# Verify environment variables are correctly updated
printf 'cd /tmp\necho "PWD after cd: $PWD"\necho "OLDPWD: $OLDPWD"\n' | ./minishell
```

**Acceptance criteria**: ✅ cd_and_pwd test already passes with updated test format.

---

### Task 3: Missing command error formatting
**Problem**: Error message format doesn't match bash exactly

**Expected bash behavior**:
```bash
$ no_such_cmd arg1
bash: line 1: no_such_cmd: command not found
```

**Current minishell behavior**:
```bash
$ no_such_cmd arg1  
command not found: no_such_cmd
```

**What to check**:
- Error message generation in executor (likely executor.c or executor_utils.c)
- Current format: "command not found: <cmd>"
- Expected format: "bash: line 1: <cmd>: command not found"
- Need to find where this error is generated and modify format

**Test commands**:
```bash
# Test current error format
printf 'nonexistent_command\n' | ./minishell 2>&1

# Compare with bash
printf 'nonexistent_command\n' | bash 2>&1

# Test with arguments
printf 'no_such_cmd arg1 arg2\n' | ./minishell 2>&1
printf 'no_such_cmd arg1 arg2\n' | bash 2>&1
```

**Acceptance criteria**: missing_cmd test passes with correct error format.

**Implementation approach**:
1. Find the location where "command not found" error is generated
2. Change format from: `"command not found: %s"`
3. To: `"minishell: line 1: %s: command not found"` (or use "bash" to match test exactly)

---

### Task 4: echo -n command parsing (moved from A)
**Problem**: Multiple commands on separate lines not executing properly in sequence

**Expected bash behavior**:
```bash
$ echo -n "no-newline"
$ printf "<END>\n"
# Results in: no-newline<END> (on same line)
```

**Current minishell behavior**: 
- echo -n works correctly (no newline printed)
- But second command (printf) doesn't execute
- Only first command executes

**What to check**:
- Parser/executor not processing multiple line commands in sequence correctly
- Each line should be parsed and executed independently
- Issue is likely in main command loop or line processing

**Test commands**:
```bash
# Test 1: Simple echo -n (should work)
echo 'echo -n hello' | ./minishell

# Test 2: Separate line commands (should both execute)
printf 'echo -n "test"\necho "after"\n' | ./minishell

# Test 3: Check if both commands run
printf 'echo "first"\necho "second"\n' | ./minishell
```

**Acceptance criteria**: echo_n test passes with both commands executing in sequence.

---

### Task 5: cd with missing operand (moved from A)
**Problem**: Custom error message when bash goes to HOME or fails silently

**Expected bash behavior**:
```bash
$ cd                # Goes to $HOME (if set) or fails silently
$ echo $?           # Exit code varies
```

**Current minishell behavior**:
```bash
$ cd                # Prints: minishell: cd: missing operand
$ echo $?           # Exit code 1
```

**What to check**:
- `bi_cd()` lines 129-133 always prints "missing operand" error
- Bash behavior: cd with no args goes to $HOME if set
- Should either implement HOME behavior or fail silently

**Test commands**:
```bash
# Test bash behavior
bash -c 'cd 2>&1; echo "Exit: $?"'

# Test current minishell behavior
./minishell <<< 'cd' 2>&1

# Test with HOME set/unset
HOME=/tmp bash -c 'cd; pwd'
unset HOME; bash -c 'cd 2>&1; echo "Exit: $?"'
```

**Acceptance criteria**: cd_missing test passes (no error output).

Engineering notes:
- Be careful not to break quoting semantics when adding # comment handling (do not strip # inside quotes).
- For PWD/OLDPWD, always set OLDPWD to previous PWD before updating PWD to getcwd(). Handle cases where getcwd() returns NULL (fallback to resolved path when possible).
- Focus on fixing command execution flow for proper sequence processing.

Definition of done for Part 2:
- make test-builtin shows all Part 2 target tests passing.
- No regressions on Part 1 tests.
- Add minimal unit helpers or inline comments documenting assumptions.

---

## Coordination plan
- Branching
  - Contributor A: feature/builtins-behavior
  - Contributor B: feature/parser-exec-fixes
- Communication
  - Share test runs by posting the full test summary after each change.
  - If touching shared files (e.g., builtins.c for cd), coordinate via PR comments to avoid conflicts.
- Merging order
  - Prefer merging Part 2 after Part 1 if both touch cd; otherwise, rebase as needed.
- Final acceptance
  - All builtin tests pass (0 FAIL) on main.

## Nice-to-haves (post-fix)
- Add a CI job to run make test-builtin on each PR.
- Add logging guard to toggle debug prints when needed (but keep disabled in tests).

