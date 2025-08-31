#!/bin/bash

# Set up a test variable
export TEST_VAR="test_value"

# Print a message
echo "Testing heredoc with mixed quotes in delimiter"

# Test the simplest case first
echo "Test 1: Unquoted delimiter"
./minishell << 'COMMAND'
cat << DELIM
$TEST_VAR should be expanded
DELIM
exit
COMMAND

echo -e "\nTest 2: Single-quoted delimiter"
./minishell << 'COMMAND'
cat << 'DELIM'
$TEST_VAR should NOT be expanded
DELIM
exit
COMMAND

echo -e "\nTest 3: Double-quoted delimiter"
./minishell << 'COMMAND'
cat << "DELIM"
$TEST_VAR should NOT be expanded
DELIM
exit
COMMAND

echo -e "\nTest 4: Mixed-quoted delimiter"
./minishell << 'COMMAND'
cat << "DE"LI"M"
$TEST_VAR should NOT be expanded
"DE"LI"M"
exit
COMMAND
