# Complete Testing Plan for Minishell Issues

## Overview
This plan investigates failing test cases by splitting them into two parts based on complexity and dependencies.

---

## Part 1: Core Variable Expansion & Path Issues
**Focus**: Basic functionality that other features depend on

### 1.1 Path and Tilde Expansion Issues
**Priority**: CRITICAL - Foundation for many other tests

#### Test Cases:
```bash
cd ~/Desktop/     # Tilde expansion not working
pwd               # Path resolution after cd
```

#### Investigation Steps:
1. **Check current tilde expansion implementation**
   - Search for `~` handling in variable expansion code
   - Location: `src/executor/expand_variables*.c`
   
2. **Test basic tilde cases**:
   ```bash
   echo ~           # Should output /home/ekakhmad
   echo ~/Desktop   # Should output /home/ekakhmad/Desktop
   cd ~             # Should change to home directory
   ```

3. **Implementation Requirements**:
   - Add tilde expansion to `expand_variables.c`
   - Handle `~` at start of word
   - Handle `~/path` patterns
   - Integrate with existing variable expansion

#### Expected Fix Location:
- `src/executor/expand_variables.c` - main expansion function
- `src/executor/expand_variables_utils.c` - utility functions

---

### 1.2 Complex Quote and Variable Mixing
**Priority**: HIGH - Affects many echo tests

#### Test Cases:
```bash
echo '"'$USER'"'                    # Mixed quotes with variables
/bin/echo '"'$USER'"'              # Same with absolute path
/bin/echo "'"'$USER'"'"            # Complex quote nesting
/bin/echo '"'"$USER"'"'            # Another complex pattern
```

#### Investigation Steps:
1. **Test individual components**:
   ```bash
   echo $USER                      # Basic variable
   echo '"test"'                   # Basic quote mixing
   echo '"'test'"'                 # Quote pattern without variable
   ```

2. **Check quote parsing logic**:
   - Location: `src/lexer/tokenizer.c`
   - Look for quote state handling
   - Check variable expansion within quotes

3. **Debug expansion order**:
   - Variables expanded before or after quote processing?
   - Are quotes properly removed after expansion?

#### Expected Fix Locations:
- `src/lexer/tokenizer.c` - quote parsing
- `src/executor/expand_variables.c` - expansion order
- `src/executor/expand_mixed_quotes.c` - quote mixing logic

---

### 1.3 Dollar Quote Syntax Issues
**Priority**: HIGH - Special bash syntax

#### Test Cases:
```bash
/bin/echo $"HOME"$USER             # Dollar quotes with variables
/bin/echo $"HOM"E$USER             # Partial dollar quotes
/bin/echo $"HOME"                  # Simple dollar quotes
/bin/echo $"42$"                   # Dollar quotes with special chars
```

#### Investigation Steps:
1. **Understand dollar quote behavior**:
   - `$"string"` should be treated as `"string"` in bash
   - Variables inside should be expanded
   - Test: `$"HOME"` vs `"$HOME"`

2. **Check current implementation**:
   - Search for `$"` handling in lexer
   - Location: `src/lexer/` and `src/executor/dollar_*.c`

3. **Test dollar quote variants**:
   ```bash
   echo $"test"                    # Basic dollar quote
   echo $"$USER"                   # Variable in dollar quote
   echo $"HOME"literal             # Dollar quote + literal
   ```

#### Expected Fix Locations:
- `src/lexer/tokenizer.c` - dollar quote tokenization
- `src/executor/dollar_quotes_fix.c` - dollar quote processing
- `src/executor/preprocess_dollar_quotes.c` - preprocessing

---

## Part 2: Advanced Features & Edge Cases
**Focus**: Complex functionality and edge cases

### 2.1 Escape Sequence Handling
**Priority**: MEDIUM - Affects specific test patterns

#### Test Cases:
```bash
/bin/echo \$USER                   # Single backslash escape
/bin/echo \\$USER                  # Double backslash
/bin/echo \\\$USER                 # Triple backslash (working!)
/bin/echo \\\\$USER                # Quad backslash
/bin/echo \\\\\$USER               # Penta backslash
/bin/echo \\\\\\\\\$USER           # Many backslashes
/bin/echo \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \$PATH \\$PWD  # Complex case
```

#### Investigation Steps:
1. **Test escape patterns systematically**:
   ```bash
   echo \a          # Literal backslash-a
   echo \\a         # Backslash + a
   echo \\\a        # Backslash + literal backslash-a
   ```

2. **Check escape processing**:
   - Location: `src/lexer/tokenizer.c`
   - When are escapes processed?
   - Interaction with variable expansion

3. **Debug why `\\\$USER` works but others don't**:
   - Count escape levels
   - Check expansion order

#### Expected Fix Locations:
- `src/lexer/tokenizer.c` - escape sequence parsing
- `src/executor/expand_variables.c` - escape handling during expansion

---

### 2.2 Complex Variable and Quote Combinations
**Priority**: MEDIUM - Advanced syntax support

#### Test Cases:
```bash
/bin/echo ""'""'""""$USER""""'""'""                    # Complex quote mixing
/bin/echo $USER'$USER'text oui oui     oui  oui $USER oui      $USER ''  # Mixed variables and literals
'''''''''''''''' echo ok                                # Many single quotes
echo '$'$'$'$'$'                                       # Dollar-quote combinations
echo '$'$'$'$'$'$'$'                                   # Extended pattern
```

#### Investigation Steps:
1. **Break down complex cases**:
   - Identify quote boundaries
   - Track variable expansion points
   - Check whitespace handling

2. **Test quote state machine**:
   - Single quotes disable everything
   - Double quotes allow variable expansion
   - Quote concatenation rules

#### Expected Fix Locations:
- `src/lexer/tokenizer.c` - complex quote parsing
- `src/executor/expand_mixed_quotes.c` - quote combination logic

---

### 2.3 Heredoc and Advanced Features
**Priority**: LOW - Secondary functionality

#### Test Cases:
```bash
cat << "$US"E"R"                   # Complex heredoc delimiter
because
we
love
bash
$USER
```

#### Investigation Steps:
1. **Test heredoc delimiter parsing**:
   - How are complex delimiters handled?
   - Variable expansion in delimiters
   - Quote handling in delimiters

2. **Check heredoc implementation**:
   - Location: `src/executor/redir_utils*.c`
   - Delimiter matching logic

---

### 2.4 Export and Variable Assignment Edge Cases
**Priority**: MEDIUM - Important for environment handling

#### Test Cases:
```bash
export T=ech; echo $T"o"           # Variable concatenation
export T="-n test1 -n test 2"; echo $T  # Special characters in variables
export T="echo segfault | grep segfault"; $T  # Command in variable
export X=" A  B  "; /bin/echo "1"$X'2'    # Whitespace preservation
```

#### Investigation Steps:
1. **Test export functionality**:
   - Variable assignment parsing
   - Special character handling
   - Whitespace preservation

2. **Check variable storage and retrieval**:
   - Location: `src/builtins/builtin_export.c`
   - Environment variable handling

---

## Testing Methodology

### Phase 1: Diagnostic Testing
1. **Create test script for each category**
2. **Compare bash vs minishell output** for each case
3. **Document exact differences**
4. **Identify root causes**

### Phase 2: Systematic Debugging
1. **Add debug output** to relevant functions
2. **Trace execution** for failing cases
3. **Identify fix points**
4. **Implement solutions incrementally**

### Phase 3: Validation
1. **Test fixes individually**
2. **Run complete test suite**
3. **Verify no regressions**
4. **Document improvements**

---

## Success Metrics

### Part 1 Success (Core Features):
- ✅ Tilde expansion working: `cd ~`, `echo ~`
- ✅ Basic quote/variable mixing: `echo '"'$USER'"'`
- ✅ Dollar quotes working: `/bin/echo $"HOME"`
- **Target**: 80%+ basic functionality tests passing

### Part 2 Success (Advanced Features):
- ✅ Escape sequences working correctly
- ✅ Complex quote combinations working
- ✅ Advanced variable handling
- **Target**: 95%+ comprehensive test coverage

---

## Implementation Priority

1. **Week 1**: Part 1 - Core functionality (tilde, basic quotes, dollar quotes)
2. **Week 2**: Part 2 - Advanced features (escapes, complex cases)
3. **Week 3**: Integration testing and optimization

This plan ensures systematic investigation and incremental improvement of minishell functionality.
