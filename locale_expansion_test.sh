#!/bin/bash

# Test script for $"string" locale expansion
# This tests how bash handles it vs minishell

echo "=== Testing locale string expansion ($\"string\") ==="
echo

echo "Test 1: echo \$\"HOME\"\$USER"
echo -n "Bash output: "
/bin/echo $"HOME"$USER
echo -n "Expected minishell output: HOME followed by username"
echo
echo

echo "Test 2: echo \$\"HOME is \$HOME\""
echo -n "Bash output: "
/bin/echo $"HOME is $HOME"
echo -n "Expected minishell output: HOME is $HOME (literally, no expansion inside locale string)"
echo
echo

echo "Test 3: echo \$\"\\$USER\""
echo -n "Bash output: "
/bin/echo $"\$USER"
echo -n "Expected minishell output: $USER (literally, no expansion inside locale string)"
echo
echo

echo "=== Now testing in minishell ==="
echo "Running: ./minishell with the same commands..."
echo

./minishell << 'EOF'
echo $"HOME"$USER
echo $"HOME is $HOME"
echo $"\$USER"
exit
EOF

echo
echo "=== Tests complete ==="
