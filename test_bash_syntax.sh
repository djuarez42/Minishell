#!/bin/bash

# This script tests the syntax of redirection commands in bash to help understand 
# how they should be implemented in minishell

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Create test directory if not exists
TESTDIR="/tmp/minishell_syntax_test"
mkdir -p $TESTDIR
touch $TESTDIR/Makefile  # Create a dummy Makefile for testing

# Function to test a command in bash
test_command_in_bash() {
    local cmd="$1"
    echo -e "${YELLOW}Testing bash syntax for: ${cmd}${RESET}"
    
    # Run in bash and capture stderr
    bash -c "$cmd" 2> $TESTDIR/bash_stderr.txt
    local bash_exit_code=$?
    
    echo -e "${YELLOW}Bash exit code: ${bash_exit_code}${RESET}"
    
    if [ -s $TESTDIR/bash_stderr.txt ]; then
        echo -e "${RED}Bash stderr output:${RESET}"
        cat $TESTDIR/bash_stderr.txt
    else
        echo -e "${GREEN}No bash stderr output.${RESET}"
    fi
    
    echo "---------------------------------------------"
}

echo -e "${YELLOW}=== TESTING REDIRECTION COMMANDS WITH POTENTIAL SYNTAX ERRORS ===${RESET}"
echo -e "${YELLOW}These tests show how bash handles these syntax issues, which can guide minishell implementation${RESET}"

# Test commands
test_command_in_bash "echo > <"
test_command_in_bash "/bin/cat ><"
test_command_in_bash "/bin/cat <$TESTDIR/Makefile >"
test_command_in_bash "echo >"
test_command_in_bash "echo > <"

# Clean up
rm -rf $TESTDIR
