#!/bin/bash

# Diagnostic script for minishell test cases
# Part 1: Core functionality testing

echo "=== MINISHELL DIAGNOSTIC TEST SCRIPT ==="
echo "Testing Part 1: Core Variable Expansion & Path Issues"
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

echo "=== PART 1.1: PATH AND TILDE EXPANSION ==="

# Basic tilde tests
compare_output "echo ~" "Basic tilde expansion"
compare_output "echo ~/Desktop" "Tilde with path"
compare_output "echo cd ~" "Tilde in echo cd"

# Path tests (after implementing tilde)
# compare_output "cd ~ && pwd" "Change to home and pwd"

echo "=== PART 1.2: QUOTE AND VARIABLE MIXING ==="

# Set up test environment
export USER="testuser"

# Basic quote/variable combinations
compare_output "echo '\"'\$USER'\"'" "Single quotes around double quotes with variable"
compare_output "/bin/echo '\"'\$USER'\"'" "Same with /bin/echo"
compare_output "/bin/echo \"'\"'\$USER'\"'\"" "Complex quote nesting pattern 1"
compare_output "/bin/echo '\"'\"'\$USER'\"'\"'" "Complex quote nesting pattern 2"

echo "=== PART 1.3: DOLLAR QUOTE SYNTAX ==="

# Dollar quote tests
compare_output "/bin/echo \$\"HOME\"\$USER" "Dollar quotes with variables"
compare_output "/bin/echo \$\"HOM\"E\$USER" "Partial dollar quotes"
compare_output "/bin/echo \$\"HOME\"" "Simple dollar quotes"
compare_output "/bin/echo \$\"42\$\"" "Dollar quotes with special chars"

echo "=== BASIC VARIABLE EXPANSION TESTS ==="

# Test basic variable expansion
compare_output "echo \$USER" "Basic variable expansion"
compare_output "echo \$?" "Exit code variable"
compare_output "echo \$HOME" "HOME variable"
compare_output "echo \$NONEXISTENT" "Non-existent variable"

echo "=== TEST COMPLETE ==="
echo "Check output above for differences between bash and minishell"
echo "Focus on fixing issues in order of priority listed in TESTING_PLAN.md"
