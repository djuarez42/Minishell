#!/bin/bash

# Test the fixed redirection

# Compile minishell
cd /home/ekakhmad/Minishell
make

# Create a test command file
echo 'echo "redirection fixed!" > redirection_fixed.txt' > test_cmd.txt

# Run minishell with the command
echo "Running minishell with redirection test..."
./minishell < test_cmd.txt

# Check if the file was created
if [ -f redirection_fixed.txt ]; then
    echo "SUCCESS: redirection_fixed.txt was created"
    echo "Contents:"
    cat redirection_fixed.txt
else
    echo "FAILURE: redirection_fixed.txt was not created"
fi
