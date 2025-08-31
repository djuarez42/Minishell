#!/bin/bash

# Get the current user's name
USER_VAR=$(whoami)

# Path to the minishell executable
MINISHELL_PATH="./minishell"

# Function to run a test case
run_test() {
    command_to_pipe=$1
    expected_output=$2
    
    echo "----------------------------------------"
    echo "Testing command: $command_to_pipe"
    
    # Run the command in minishell by piping it
    actual_output=$(echo "$command_to_pipe" | $MINISHELL_PATH)
    
    # The prompt might be part of the output, so remove it if present
    actual_output=${actual_output#"minishell$ "}

    echo "Expected: '$expected_output'"
    echo "Actual  : '$actual_output'"
    
    if [ "$actual_output" == "$expected_output" ]; then
        echo "✅ PASSED"
    else
        echo "❌ FAILED"
    fi
}

# Recompile minishell
echo "Recompiling minishell..."
make
if [ $? -ne 0 ]; then
    echo "Compilation failed. Aborting tests."
    exit 1
fi
echo "Compilation successful."

# --- Test Cases ---

# /bin/echo $USER -> Should print literal $USER (not expand)
run_test '/bin/echo \$USER' '$USER'

# /bin/echo \$USER -> Should print \ekakhmad (one backslash + expanded USER)
run_test '/bin/echo \\$USER' "\\$USER_VAR"

# /bin/echo \\$USER -> Should print \$USER (one backslash + literal $USER)
run_test '/bin/echo \\\$USER' '\\$USER'

# /bin/echo \\\$USER -> Should print \\ekakhmad (two backslashes + expanded USER)
run_test '/bin/echo \\\\$USER' "\\\\$USER_VAR"

# /bin/echo \\\\$USER -> Should print \\$USER (two backslashes + literal $USER)
run_test '/bin/echo \\\\\\$USER' '\\\\$USER'

# /bin/echo \\\\\$USER -> Should print \\\\$USER (four backslashes + literal $USER)
run_test '/bin/echo \\\\\\\\$USER' '\\\\\\\\$USER'

echo "----------------------------------------"
echo "All tests completed."
