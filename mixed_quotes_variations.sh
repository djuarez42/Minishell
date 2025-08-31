#!/bin/bash

# Set up test variable
export TEST_VAR="test_value"

# Test different mixed quote patterns
echo "=== Test 1: Mixed double quotes (\"D\"ELIM) ==="
./minishell << 'COMMAND'
cat << "D"ELIM
$TEST_VAR should NOT be expanded
DELIM
exit
COMMAND

echo -e "\n=== Test 2: Mixed single quotes ('D'ELIM) ==="
./minishell << 'COMMAND'
cat << 'D'ELIM
$TEST_VAR should NOT be expanded
DELIM
exit
COMMAND

echo -e "\n=== Test 3: Alternating quotes (\"D\"'E'\"L\"'I'\"M\") ==="
./minishell << 'COMMAND'
cat << "D"'E'"L"'I'"M"
$TEST_VAR should NOT be expanded
DELIM
exit
COMMAND

echo -e "\n=== Test 4: Variable in quotes (\"$TEST_VAR\") ==="
./minishell << 'COMMAND'
cat << "$TEST_VAR"
$TEST_VAR should NOT be expanded
test_value
exit
COMMAND
