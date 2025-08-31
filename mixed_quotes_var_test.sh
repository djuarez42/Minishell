#!/bin/bash

# Set up a variable with spaces
export X="a b c"

# Create a temporary file for minishell commands
CMDS_FILE="mixed_quotes_var_cmds.txt"

# Test cases to run
declare -a test_cases=(
  'echo "1"$X"2"'
  'echo "1"$X'"'"'2'"'"''
  'echo '"'"'1'"'"'$X"2"'
  'echo '"'"'1'"'"'$X'"'"'2'"'"''
  'echo $X'
  'echo "$X"'
  'echo '"'"'$X'"'"''
)

echo "===== TESTING MIXED QUOTES WITH VARIABLE EXPANSION ====="
echo "Variable X=$X (contains spaces that should be preserved in quotes)"
echo ""

for cmd in "${test_cases[@]}"; do
  echo "--- TEST CASE: $cmd ---"
  
  # Run in bash
  echo "BASH OUTPUT:"
  bash_output=$(eval "$cmd")
  echo "$bash_output"
  
  # Run in minishell
  echo "MINISHELL OUTPUT:"
  echo "$cmd" > "$CMDS_FILE"
  minishell_output=$(./minishell < "$CMDS_FILE")
  echo "$minishell_output"
  
  # Compare outputs
  if [ "$bash_output" = "$minishell_output" ]; then
    echo "✅ PASS: Outputs match"
  else
    echo "❌ FAIL: Outputs differ"
    echo "Expected: '$bash_output'"
    echo "Got:      '$minishell_output'"
  fi
  echo ""
done

# Clean up
rm -f "$CMDS_FILE"
