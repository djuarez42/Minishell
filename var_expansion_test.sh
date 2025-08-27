#!/bin/bash

# Test variable expansion in your minishell

echo "Testing variable expansion in minishell"
echo "========================================"

cd "$(dirname "$0")"

# Test simple variable expansion
echo -e "\nTest 1: Simple variable expansion"
echo 'export X=" " && /bin/echo "1"$X"2"' | ./minishell

# Test variable expansion with quotes
echo -e "\nTest 2: Variable expansion with single quotes"
echo 'export X=" " && /bin/echo "1"$X'"'"'2'"'"'' | ./minishell

# Test variable expansion with more complex expressions
echo -e "\nTest 3: Complex variable expansion"
echo 'export FOO=bar && /bin/echo "$FOO" '\''$FOO'\'' $FOO' | ./minishell

echo -e "\nTest 4: Variable expansion from environment"
echo 'export PATH2=$PATH && /bin/echo $PATH2 | wc -c' | ./minishell

echo -e "\nTest 5: Nested variable expansion"
echo 'export A=B && export B=C && /bin/echo $A and $$A' | ./minishell

echo -e "\nDone!"
