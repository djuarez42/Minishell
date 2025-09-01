#!/bin/bash

# Test if output redirection is working correctly
cd /home/ekakhmad/Minishell

# Run the minishell with a simple command and redirect output to a file
echo "echo hello world" > test_cmd.txt
./minishell < test_cmd.txt > minishell_output.txt

echo "=== Content of minishell_output.txt ==="
cat minishell_output.txt
echo "========================================="

# Test with bash for comparison
bash < test_cmd.txt > bash_output.txt

echo "=== Content of bash_output.txt ==="
cat bash_output.txt
echo "========================================="

# Test direct output
echo -e "\n=== Direct command test ==="
echo "Running: ./minishell -c \"echo hello\""
./minishell -c "echo hello" 2>&1 || echo "Command failed with exit code $?"
