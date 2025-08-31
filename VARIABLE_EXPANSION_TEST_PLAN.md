# Variable Expansion Test Plan

## Test Categories

### 1. Simple Variable Expansion

| Test Case | Expected Behavior | Bash Output | Notes |
|-----------|-------------------|-------------|-------|
| `$HOME` | Expands to home directory | `/home/user` | Basic variable expansion |
| `$USER` | Expands to username | `username` | Basic variable expansion |
| `$NOTFOUND` | Expands to empty string | `` | Non-existent variable |
| `$?` | Expands to last exit status | `0` | Special variable |
| `$$` | Expands to process ID | `12345` | Special variable |

### 2. Variables Adjacent to Text

| Test Case | Expected Behavior | Bash Output | Notes |
|-----------|-------------------|-------------|-------|
| `$HOMEdir` | Should not find a variable named "HOMEdir" | `` | Adjacent text |
| `prefix$USER` | "prefix" + value of USER | `prefixusername` | Text before variable |
| `$HOME$USER` | HOME value + USER value | `/home/userusername` | Two variables |
| `$HOME$NOTFOUND$USER` | HOME + empty + USER | `/home/userusername` | With non-existent var |

### 3. Quoted Variables

| Test Case | Expected Behavior | Bash Output | Notes |
|-----------|-------------------|-------------|-------|
| `"$HOME"` | Expands inside double quotes | `/home/user` | Double quotes allow expansion |
| `'$HOME'` | Literal string, no expansion | `$HOME` | Single quotes prevent expansion |
| `"$HOME$USER"` | Both variables expand | `/home/userusername` | Multiple vars in double quotes |
| `'$HOME$USER'` | Literal string, no expansion | `$HOME$USER` | Multiple vars in single quotes |

### 4. ANSI-C Quoted Strings

| Test Case | Expected Behavior | Bash Output | Notes |
|-----------|-------------------|-------------|-------|
| `$'HOM'` | ANSI-C quoted string "HOM" | `HOM` | Basic ANSI-C quoting |
| `$'\n'` | Newline character | `[newline]` | ANSI-C escape sequence |
| `$'HOM'E` | "HOM" + "E" | `HOME` | ANSI-C quote plus literal |
| `$'HOM'$USER` | "HOM" + USER value | `HOMusername` | ANSI-C quote plus variable |
| `$'HOM'E$USER` | "HOM" + "E" + USER value | `HOMEusername` | Complex case |

### 5. Localization Strings

| Test Case | Expected Behavior | Bash Output | Notes |
|-----------|-------------------|-------------|-------|
| `$"HOME"` | "HOME" (no localization in simple implementation) | `HOME` | Localization string |
| `$"HOM"E` | "HOM" + "E" | `HOME` | Localization string plus literal |
| `$"HOM"$USER` | "HOM" + USER value | `HOMusername` | Localization string plus variable |
| `$"HOM"E$USER` | "HOM" + "E" + USER value | `HOMEusername` | Complex case |

### 6. Mixed and Edge Cases

| Test Case | Expected Behavior | Bash Output | Notes |
|-----------|-------------------|-------------|-------|
| `$` | Literal "$" | `$` | Just a dollar sign |
| `$"` | Literal "$" + `"` | `$"` | Unclosed localization string |
| `$'` | Literal "$" + `'` | `$'` | Unclosed ANSI-C quoted string |
| `$HALLO42ho` | Empty string (non-existent var) | `` | Variable with suffix |
| `$'HOM'E$NOTFOUND$USER` | "HOM" + "E" + empty + USER value | `HOMEusername` | Complex mixed case |

## Test Procedure

1. For each test case:
   - Run the command in Bash and record the output
   - Run the same command in Minishell and record the output
   - Compare the outputs

2. For failing tests:
   - Identify the specific aspect that's failing
   - Check the related code in Minishell
   - Apply the necessary fixes based on the implementation plan

## Automated Testing Script

Create a script that:
1. Runs each test case in both Bash and Minishell
2. Compares the outputs
3. Reports any differences

Example:
```bash
#!/bin/bash

run_test() {
    local test_case="$1"
    local description="$2"
    
    echo "Testing: $description"
    echo "Command: $test_case"
    
    # Run in Bash
    echo -n "Bash output: "
    bash_output=$(bash -c "echo $test_case")
    echo "$bash_output"
    
    # Run in Minishell
    echo -n "Minishell output: "
    minishell_output=$(echo "echo $test_case" | ./minishell)
    echo "$minishell_output"
    
    # Compare
    if [ "$bash_output" = "$minishell_output" ]; then
        echo "✅ PASS"
    else
        echo "❌ FAIL"
    fi
    echo
}

# Simple Variable Expansion
run_test '$HOME' "Basic variable expansion (HOME)"
run_test '$USER' "Basic variable expansion (USER)"
run_test '$NOTFOUND' "Non-existent variable"
run_test '$?' "Special variable (exit status)"
run_test '$$' "Special variable (process ID)"

# More test cases...
```

## Integration with Existing Test Framework

The 42_minishell_tester already has test cases for variables. Once the fixes are implemented:

1. Run the full test suite
2. Focus on the specific cases that were failing before
3. Verify that all variable expansion tests now pass
