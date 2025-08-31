#!/bin/bash

# Set up a test file
cat > test_input.txt << 'TEST_END'
/bin/echo "$"$
exit
TEST_END

# Run the minishell with the test input
./minishell < test_input.txt

# Run the actual bash command to see expected output
echo "Expected output from bash:"
/bin/echo "$"$

# Clean up
rm test_input.txt
