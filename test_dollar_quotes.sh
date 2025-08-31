#!/bin/bash
# Test script for special dollar quote handling in minishell

echo "=== Testing in bash ==="
echo "echo \$\"HOME\""
echo $"HOME"
echo "echo \$\'HOME\'"
echo $'HOME'
echo "echo \$\"HOME\"\$USER"
echo $"HOME"$USER

echo -e "\n=== Testing in minishell ==="
echo "echo \$\"HOME\""
echo "echo \$\'HOME\'"
echo "echo \$\"HOME\"\$USER"

# Now run through minishell
echo -e "\nRunning same commands through minishell:"
echo "echo \$\"HOME\"" | ./minishell
echo "echo \$\'HOME\'" | ./minishell
echo "echo \$\"HOME\"\$USER" | ./minishell

exit 0
