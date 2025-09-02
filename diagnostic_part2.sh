#!/bin/bash

# Diagnostic script for minishell test cases
# Part 2: Advanced features and edge cases

echo "=== MINISHELL DIAGNOSTIC TEST SCRIPT ==="
echo "Testing Part 2: Advanced Features & Edge Cases"
echo

# Function to compare bash vs minishell output
compare_output() {
    local test_cmd="$1"
    local test_name="$2"
    
    echo "----------------------------------------"
    echo "TEST: $test_name"
    echo "COMMAND: $test_cmd"
    echo
    
    # Get bash output
    echo "BASH OUTPUT:"
    bash -c "$test_cmd" 2>&1
    local bash_exit=$?
    echo "[EXIT CODE: $bash_exit]"
    echo
    
    # Get minishell output  
    echo "MINISHELL OUTPUT:"
    echo "$test_cmd" | timeout 3 ../minishell 2>&1
    local ms_exit=$?
    echo "[EXIT CODE: $ms_exit]"
    echo
    
    # Compare exit codes
    if [ $bash_exit -eq $ms_exit ]; then
        echo "EXIT CODE: ✅ MATCH"
    else
        echo "EXIT CODE: ❌ DIFFERENT (bash: $bash_exit, minishell: $ms_exit)"
    fi
    echo
}

# Create test directory
mkdir -p /tmp/minishell_test
cd /tmp/minishell_test

echo "=== PART 2.1: ESCAPE SEQUENCE HANDLING ==="

# Escape sequence tests
compare_output "/bin/echo \\\$USER" "Single backslash escape"
compare_output "/bin/echo \\\\\$USER" "Double backslash"
compare_output "/bin/echo \\\\\\\$USER" "Triple backslash (should work)"
compare_output "/bin/echo \\\\\\\\\$USER" "Quad backslash"
compare_output "/bin/echo \\\\\\\\\\\$USER" "Penta backslash"
compare_output "/bin/echo \\\\\\\\\\\\\\\\\\\$USER" "Many backslashes"

# Complex escape case
compare_output "/bin/echo \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \\\$PATH \\\\\\\$PWD" "Complex escape pattern"

echo "=== PART 2.2: COMPLEX QUOTE COMBINATIONS ==="

# Set up test environment  
export USER="testuser"

# Complex quote mixing
compare_output "/bin/echo \"\"'\"\"'\"\"\"\"\\$USER\"\"\"\"'\"\"'\"\"" "Complex quote mixing with variables"
compare_output "/bin/echo \\$USER'\\$USER'text oui oui     oui  oui \\$USER oui      \\$USER ''" "Mixed variables and literals"

# Many single quotes
compare_output "''''''''''''''' echo ok" "Many single quotes with command"

# Dollar-quote combinations
compare_output "echo '\\$'\\$'\\$'\\$'\\$'" "Dollar-quote combinations pattern 1"
compare_output "echo '\\$'\\$'\\$'\\$'\\$'\\$'\\$'" "Dollar-quote combinations pattern 2"

echo "=== PART 2.3: EXPORT AND VARIABLE ASSIGNMENT ==="

# Export with concatenation
compare_output "export T=ech; echo \\$T\"o\"" "Variable concatenation"
compare_output "export T=\"-n test1 -n test 2\"; echo \\$T" "Special characters in variables"

# Whitespace preservation tests
compare_output "export X=\" A  B  \"; /bin/echo \"1\"\\$X'2'" "Whitespace preservation 1"
compare_output "export X=\"A  B  \"; /bin/echo \"1\"\\$X'2'" "Whitespace preservation 2"
compare_output "export X=\"  A  B \"; /bin/echo \"1\"\\$X'2'" "Whitespace preservation 3"
compare_output "export X=\"  A  B\"; /bin/echo \"1\"\\$X'2'" "Whitespace preservation 4"
compare_output "export X=\"  A B  \"; /bin/echo \"1\"\\$X'2'" "Whitespace preservation 5"
compare_output "export X=\"  AB  \"; /bin/echo \"1\"\\$X'2'" "Whitespace preservation 6"

# More export tests
compare_output "export X=\"  A  B  \"; /bin/echo \\$X'2'" "Variable with whitespace + literal"
compare_output "export X=\"  A  B  \"; /bin/echo \\$X\"1\"" "Variable with whitespace + quotes"
compare_output "export X=\"\"; /bin/echo \"1\"\\$X'2'" "Empty variable"
compare_output "export X=\" \"; /bin/echo \"1\"\\$X'2'" "Space-only variable"
compare_output "export X=\"   \"; /bin/echo \"1\"\\$X'2'" "Multiple spaces variable"

echo "=== PART 2.4: SPECIAL CASES ==="

# Exit code tests
compare_output "/bin/echo \"\"\\$?\"\"" "Exit code in double quotes"
compare_output "/bin/echo \" \"\\$?\" \"" "Exit code with spaces"
compare_output "/bin/echo \\$?\"42\"" "Exit code concatenated with literal"
compare_output "/bin/echo ''\\$?''\"42\"" "Exit code with mixed quotes"

# Non-existent variable
compare_output "/bin/echo \\$TESTNOTFOUND" "Non-existent variable"

# Quote literals with pipes (should be treated as literals)
compare_output "echo 1 '|' echo 2" "Pipe in quotes (literal)"
compare_output "echo 1 '&&' echo 2" "AND in quotes (literal)"
compare_output "echo 1 '||' echo 2" "OR in quotes (literal)" 
compare_output "echo 1 '>' echo 2" "Redirect in quotes (literal)"

echo "=== PART 2.5: DOLLAR QUOTE EDGE CASES ==="

# More dollar quote tests
compare_output "/bin/echo \\$\"'HOM'E\"\\$USER" "Dollar quotes with single quotes inside"
compare_output "/bin/echo \\$'HOM'E\\$USER" "Single dollar quote"
compare_output "/bin/echo \\$\"HOME\"" "Simple dollar quote"
compare_output "/bin/echo \\$'HOME'" "Simple single dollar quote"
compare_output "/bin/echo \"\\$\"\"\\$\"" "Dollar in double quotes"
compare_output "/bin/echo \\$\"\\$\"" "Dollar in dollar quotes"
compare_output "/bin/echo \"\\$\"\\$" "Mixed dollar patterns"

echo "=== TEST COMPLETE ==="
echo "Check output above for differences between bash and minishell"
echo "Part 2 focuses on advanced edge cases and complex syntax"
