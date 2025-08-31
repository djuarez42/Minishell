#!/bin/bash

# Set up test variable
export TEST_VAR="test_value"

# Test only mixed quotes
echo "Testing mixed quotes in heredoc delimiter..."
echo -e "Running: cat << \"D\"E\"L\"I\"M\"\n"

./minishell << 'COMMAND'
cat << "D"E"L"I"M"
$TEST_VAR should NOT be expanded
DELIM
exit
COMMAND

echo -e "\nTesting complete"
