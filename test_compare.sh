#!/bin/bash

# Create test directory
mkdir -p test_minishell
cd test_minishell

# File for comparison results
touch bash_vs_minishell.txt

# Run tests comparing bash and minishell
run_test() {
    local cmd="$1"
    local test_name="$2"
    
    echo "=== Test: $test_name ===" >> bash_vs_minishell.txt
    echo "Command: $cmd" >> bash_vs_minishell.txt
    
    # Run in bash
    echo "Bash output:" >> bash_vs_minishell.txt
    echo "$cmd" | bash 2>> bash_vs_minishell.txt 1>> bash_vs_minishell.txt
    echo "Bash exit code: $?" >> bash_vs_minishell.txt
    
    # Run in minishell
    echo "Minishell output:" >> bash_vs_minishell.txt
    echo "$cmd" | ../minishell 2>> bash_vs_minishell.txt 1>> bash_vs_minishell.txt
    echo "Minishell exit code: $?" >> bash_vs_minishell.txt
    
    echo "" >> bash_vs_minishell.txt
}

# Basic echo tests
run_test "echo hello world" "Basic echo"
run_test "echo 'hello world'" "Echo with single quotes"
run_test "echo \"hello world\"" "Echo with double quotes"
run_test "echo \$USER" "Echo environment variable"
run_test "echo \$'USER'" "ANSI-C Quote echo"
run_test "echo \$\"USER\"" "Locale-specific translation"
run_test "echo \$HOME" "Echo HOME variable"

# Variable expansion tests
run_test "TEST=hello; echo \$TEST" "Variable assignment and echo"
run_test "echo \$NONEXISTENT" "Echo non-existent variable"
run_test "echo \"\$USER\"" "Echo variable in double quotes"
run_test "echo '\$USER'" "Echo variable in single quotes"

# Display results
echo "Comparison completed. Results saved to bash_vs_minishell.txt"
cat bash_vs_minishell.txt
