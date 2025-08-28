# 🔧 Debug Plan: Systematic Testing for Optimal Solutions

## 📊 Current Status
- ✅ **75% test cases working** (12/16)
- ✅ **Tokenizer/Parser: 100% functional**
- ✅ **Basic expansion: 100% functional**
- ❌ **Redirections: 0% functional** (complete failure)
- ❌ **Dollar quotes: edge case bugs**

## 🎯 Priority 1: Redirection Debug Plan

### Problem Analysis
**All redirections fail**: `echo hello > file.txt` creates no file

### Hypothesis Testing Strategy

#### Test 1: Execution Path Tracing
```bash
# Test simple command (works)
echo 'echo hello' | ./minishell

# Test redirection (fails)  
echo 'echo hello > test.txt' | ./minishell

# Question: Where does execution diverge?
```

#### Test 2: Command Structure Analysis
```bash
# Test if command parsing is correct
echo 'pwd' | ./minishell  # Should work
echo 'pwd > test.txt' | ./minishell  # Should create file
```

#### Test 3: Redirection Type Isolation
```bash
# Test different redirection types
echo 'echo test > out.txt' | ./minishell    # OUTPUT
echo 'cat < /etc/hosts' | ./minishell       # INPUT  
echo 'echo test >> app.txt' | ./minishell   # APPEND
```

### Investigation Steps

1. **Step 1**: Verify command parsing creates correct AST
2. **Step 2**: Check if `expand_redirs()` is being called
3. **Step 3**: Verify `handle_redirections()` receives correct data
4. **Step 4**: Check if file operations are attempted
5. **Step 5**: Test error handling and file creation

### Expected Findings
- **Likely issue**: Execution flow broken after flowchart changes
- **Possible causes**: 
  - Redirection expansion happening too late/early
  - Memory corruption in file paths
  - Missing function calls in execution pipeline

## 🎯 Priority 2: Dollar Quote Debug Plan

### Problem Analysis
- `$"HOME"$USER` → `/Users/ekaterinaekaterina` (should be `$HOMEekaterina`)
- `$""$USER` → `$USER` (should be `$ekaterina`) 
- `$"test"` → empty (should be `$test`)

### Hypothesis Testing Strategy

#### Test 1: Basic Dollar Behavior Verification
```bash
# Verify bash behavior
echo $"HOME"     # Should output: $HOME (in bash)
echo $"test"     # Should output: $test (in bash)
echo $""         # Should output: $ (in bash)
```

#### Test 2: Tokenization vs Expansion Issue
```bash
# Test if issue is in tokenization or expansion
echo 'echo $USER'     # Works: ekaterina
echo 'echo "$USER"'   # Should work: ekaterina  
echo 'echo $"USER"'   # Broken: should be $USER
```

#### Test 3: Position Tracking Issue
```bash
# Test continuation processing
echo 'echo $"a"x'     # Should be: $ax
echo 'echo $""x'      # Should be: $x
echo 'echo $""$USER'  # Should be: $ekaterina
```

### Investigation Steps

1. **Step 1**: Confirm bash behavior for all cases
2. **Step 2**: Check `handle_dollar()` logic for `$"` cases
3. **Step 3**: Verify position advancement in expansion loop
4. **Step 4**: Test variable expansion continuation after quotes

## 📋 Test Implementation Plan

### Phase 1: Redirection Deep Dive (30 min)
1. Create minimal test cases
2. Add debug output to trace execution
3. Identify exact failure point
4. Implement targeted fix

### Phase 2: Dollar Quote Fix (20 min)  
1. Verify bash behavior reference
2. Fix `handle_dollar()` logic
3. Test continuation cases
4. Validate all edge cases

### Phase 3: Integration Testing (10 min)
1. Run full test suite
2. Verify no regressions
3. Test complex combinations
4. Final validation

## 🔬 Debugging Tools

### Temporary Debug Macros
```c
#define DEBUG_REDIR 1
#ifdef DEBUG_REDIR
    fprintf(stderr, "DEBUG: %s:%d - %s\n", __FILE__, __LINE__, msg);
#endif
```

### Test Harness
```bash
#!/bin/bash
# test_runner.sh
echo "=== Redirection Tests ==="
test_redirection() {
    echo "$1" | ./minishell
    if [ -f "$2" ]; then
        echo "✅ File created: $2"
        cat "$2"
        rm -f "$2"
    else
        echo "❌ File not created: $2"
    fi
}

test_redirection "echo hello > test.txt" "test.txt"
test_redirection "pwd > pwd.txt" "pwd.txt"
```

## 🎯 Success Criteria

### Phase 1 Success
- ✅ Basic redirection works: `echo hello > file.txt` creates file with "hello"
- ✅ Quoted redirection works: `echo test > "my file.txt"`

### Phase 2 Success  
- ✅ `$"HOME"$USER` → `$HOMEekaterina`
- ✅ `$""$USER` → `$ekaterina`
- ✅ `$"test"` → `$test`

### Final Success
- ✅ **90%+ test cases working** (14/16 or better)
- ✅ **All core functionality stable**
- ✅ **Ready for production use**

## 🚀 Implementation Order

1. **Immediate**: Start with redirection debug (highest impact)
2. **Next**: Fix dollar quote logic (refinement)  
3. **Final**: Integration testing and validation

This systematic approach will identify the root causes quickly and provide optimal solutions for both problem areas.
