# Known Issues - Minishell

## Current Status
- **Tests Passed**: 261/297 (87.9%)
- **Stderr Errors**: 9 remaining
- **Exit Code Errors**: 11 remaining

## Critical Issue: Variable Expansion with External Commands

### Problem Description
When undefined variables are expanded in external command arguments, the command execution fails with exit code 1 instead of succeeding with exit code 0 as bash does.

### Reproduction
```bash
# Expected behavior (bash):
$ /bin/echo $UNDEFINED_VAR
$ echo $?
0

# Current behavior (minishell):
$ echo '/bin/echo $UNDEFINED_VAR' | ./minishell
$ echo $?
1
```

### Impact
- Affects export-related test cases (e.g., test #147 at line 376)
- Causes STD_OUT mismatches when undefined variables should expand to empty strings
- Multiple test failures in builtin tests

### Technical Analysis
1. The command `/bin/echo $UNDEFINED_VAR` should:
   - Expand `$UNDEFINED_VAR` to empty string
   - Execute `/bin/echo` with no arguments
   - Output a single newline
   - Return exit code 0

2. Current behavior:
   - Variable expansion appears to work (empty string)
   - Command execution fails somewhere in the pipeline
   - Returns exit code 1
   - No output produced

### Investigation Areas
1. **Variable Expansion Pipeline** (`src/executor/expand_variables*.c`)
   - Check if empty expansion results are handled correctly
   - Verify argument array construction after expansion

2. **Command Execution** (`src/executor/executor.c`)
   - Examine external command execution path
   - Check argument passing to execve

3. **Parser/Lexer** (`src/parser/`, `src/lexer/`)
   - Verify command structure after variable expansion
   - Check if empty arguments cause parsing issues

### Next Steps
1. Add debug output to trace variable expansion process
2. Examine argument array construction after expansion
3. Test with strace to see if execve is called with correct arguments
4. Compare argument handling between builtin and external commands

## Minor Issues

### Stderr Output Mismatches (9 remaining)
- Some error messages don't exactly match bash format
- Need to review specific test cases for exact message format

### Exit Code Mismatches (11 remaining)
- Likely related to the main variable expansion issue
- Some edge cases in builtin error handling

## Recent Fixes Completed
- ✅ Heredoc implementation in non-interactive mode
- ✅ Debug output cleanup (reduced stderr errors from 201 to 9)
- ✅ Unset builtin bash compatibility
- ✅ Tokenization improvements for pipe operators
- ✅ Test compatibility improvements (+174 passed tests)

## Redirection Issues

### 1. Heredoc Syntax Parsing Error
**Problem**: Commands like `<<lim cat` are incorrectly parsed, causing syntax errors.

**Expected behavior (bash)**:
```bash
$ <<lim cat
> content
> lim
content
```

**Current behavior (minishell)**:
```bash
$ <<lim cat
minishell: syntax error near unexpected token `|'
```

**Impact**: Affects heredoc tests where delimiter and command are on same line
**File**: `/cmds/mand/1_redirs.sh` around line 195

### 2. Standalone Redirection Operators
**Problem**: Redirections without commands produce wrong error messages.

**Expected behavior (bash)**:
```bash
$ < nonexistent_file
bash: line 1: nonexistent_file: No such file or directory
```

**Current behavior (minishell)**:
```bash
$ < nonexistent_file  
minishell: syntax error near unexpected token `|'
```

**Impact**: Error message format doesn't match bash
**Files**: Various redirection tests with `< out`, `> out`, `>> out`

### 3. External Command Redirection
**Problem**: Similar to variable expansion issue, external commands with redirections may fail.

**Example**: `/bin/echo 42 > tmp_redir_out` 
**Status**: Needs investigation - may be related to main variable expansion issue

## Investigation Priority
1. **High**: Variable expansion with external commands (affects many tests)
2. **Medium**: Heredoc parsing syntax (`<<delimiter command`)
3. **Low**: Standalone redirection error messages
