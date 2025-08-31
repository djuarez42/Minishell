#!/bin/bash

# Special test case for mixed quotes with variables expansion
# Focus on the specific test cases that are failing

# Set up environment
export X="a b c"

# Create a file with a simple command using the mixed quotes pattern
echo 'echo "1"$X"2"' > test_mixed.txt

echo "Testing: echo \"1\"\$X\"2\""
echo "Expected result from bash: 1a b c2"

# Run in minishell
echo "Running in minishell:"
./minishell < test_mixed.txt

# Test with single quotes
echo 
echo 'echo "1"$X'"'"'2'"'"'' > test_mixed.txt
echo "Testing: echo \"1\"\$X'2'"
echo "Expected result from bash: 1a b c2"
echo "Running in minishell:"
./minishell < test_mixed.txt

# Clean up
rm test_mixed.txt
