#!/bin/bash

# Quote and Variable Mixing Test Script
# Run this to establish baseline behavior before fixes

echo "=== QUOTE AND VARIABLE MIXING TESTS ==="
echo "Testing current minishell behavior vs bash"
echo

# Test Track A cases (Quote parsing - no variables)
echo "--- Track A: Quote Parsing Tests ---"
echo "Test 1: Basic quote mixing"
echo 'Input: echo '"'"'"test"'"'"
echo "Bash output:"
bash -c 'echo '"'"'"test"'"'"
echo "Minishell output:"
echo 'echo '"'"'"test"'"'"' | ./minishell
echo

echo "Test 2: Complex quote nesting"
echo 'Input: echo "'"'"'inner'"'"'"'
echo "Bash output:"
bash -c 'echo "'"'"'inner'"'"'"'
echo "Minishell output:" 
echo 'echo "'"'"'inner'"'"'"' | ./minishell
echo

# Test Track B cases (Variable expansion)
echo "--- Track B: Variable Expansion Tests ---"
echo "Test 3: Basic variable in quotes"
echo 'Input: echo '"'"'$USER'"'"
echo "Bash output:"
bash -c 'echo '"'"'$USER'"'"
echo "Minishell output:"
echo 'echo '"'"'$USER'"'"' | ./minishell
echo

echo "Test 4: Dollar quotes"
echo 'Input: /bin/echo $"HOME"'
echo "Bash output:"
bash -c '/bin/echo $"HOME"'
echo "Minishell output:"
echo '/bin/echo $"HOME"' | ./minishell
echo

# Test Combined cases
echo "--- Combined: Integration Tests ---"
echo "Test 5: Complex mixing"
echo 'Input: /bin/echo '"'"'$USER'"'"
echo "Bash output:"
bash -c '/bin/echo '"'"'$USER'"'"
echo "Minishell output:"
echo '/bin/echo '"'"'$USER'"'"' | ./minishell
echo

echo "Test 6: Dollar quote with variable"
echo 'Input: /bin/echo $"HOME"$USER'
echo "Bash output:"
bash -c '/bin/echo $"HOME"$USER'
echo "Minishell output:"
echo '/bin/echo $"HOME"$USER' | ./minishell
echo

echo "=== BASELINE COMPLETE ==="
echo "Use this output to track progress during development"
