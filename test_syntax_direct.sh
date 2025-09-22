#!/bin/bash

# Helper function to run a test and display results
test_command() {
  local cmd="$1"
  echo -e "\n\033[1;33m=== Testing: $cmd ===\033[0m"
  
  # Execute command with valgrind
  echo -e "\033[1;36mRunning with valgrind...\033[0m"
  valgrind --leak-check=full --show-leak-kinds=all ./minishell -c "$cmd" 2>&1 | grep -E '(ERROR SUMMARY|definitely lost:|indirectly lost:|possibly lost:|still reachable:|minishell:)'
  
  # Execute command normally to show stderr output
  echo -e "\n\033[1;36mRunning normally (to check stderr)...\033[0m"
  ./minishell -c "$cmd" 2>&1
  
  echo -e "\033[1;32m=== Test completed ===\033[0m"
}

# Test each command
test_command "echo > <"
test_command "/bin/cat ><"
test_command "/bin/cat <Makefile >"
test_command "echo >"

echo -e "\n\033[1;34mAll tests completed\033[0m"
