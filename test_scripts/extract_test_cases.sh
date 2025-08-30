#!/bin/bash

# Script to extract failing test cases from tester output
# and show the actual test command that failed, including the output comparison

OUTPUT_FILE="$1"
TEST_TYPE="$2"  # Can be "STD_OUT", "STD_ERR", or "EXIT_CODE"
RESULTS_DIR="test_results"  # Directory to store results
MINISHELL_PATH="../minishell"  # Path to minishell executable (adjust if needed)

if [ -z "$OUTPUT_FILE" ]; then
    echo "Usage: $0 <output_file> [test_type]"
    echo "Example: $0 pa2.txt STD_OUT"
    echo "Default test type is STD_OUT if not specified"
    exit 1
fi

# Default to STD_OUT if not specified
if [ -z "$TEST_TYPE" ]; then
    TEST_TYPE="STD_OUT"
fi

# Check if file exists
if [ ! -f "$OUTPUT_FILE" ]; then
    echo "Error: File $OUTPUT_FILE not found!"
    exit 1
fi

# Create results directory if it doesn't exist
mkdir -p "$RESULTS_DIR"

# Generate timestamp for filenames
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULTS_FILE="$RESULTS_DIR/${TEST_TYPE}_failures_$TIMESTAMP.txt"
SUMMARY_FILE="$RESULTS_DIR/${TEST_TYPE}_summary_$TIMESTAMP.md"

# Extract failing test cases
echo "=== Extracting failing $TEST_TYPE tests from $OUTPUT_FILE ==="
echo

# Create the header for the summary markdown file
{
    echo "# Minishell Test Failures: $TEST_TYPE"
    echo
    echo "**Date:** $(date)"
    echo "**Source:** $OUTPUT_FILE"
    echo
    echo "## Summary of Failing Tests"
    echo
    echo "| Test # | File | Line | Command | Status |"
    echo "|--------|------|------|---------|--------|"
} > "$SUMMARY_FILE"

# First, collect all the failing tests for the list at the beginning
echo "=== List of All Failing Tests ===" > "$RESULTS_FILE"
echo >> "$RESULTS_FILE"

# Create arrays to store test info
declare -a test_nums
declare -a test_files
declare -a test_lines
declare -a test_cmds
declare -a file_paths

# First pass to collect all test info
while read -r line; do
    # Extract test file path and line number
    file_path=$(echo "$line" | grep -o '/home/.*:[0-9]*')
    
    if [ -n "$file_path" ]; then
        # Split file_path into file and line number
        test_file=$(echo "$file_path" | cut -d':' -f1)
        line_num=$(echo "$file_path" | cut -d':' -f2)
        
        # Get test number
        test_num=$(echo "$line" | grep -o '^ *[0-9]*:' | tr -d '[:space:]' | tr -d ':')
        
        if [ -f "$test_file" ]; then
            # Extract the command
            cmd=$(sed -n "${line_num}p" "$test_file" | sed 's/^[[:space:]]*//')
            
            # Store all the info
            test_nums+=("$test_num")
            test_files+=("$test_file")
            test_lines+=("$line_num")
            test_cmds+=("$cmd")
            file_paths+=("$file_path")
            
            # Add one-line summary
            echo "Test #$test_num: $cmd ($file_path)" >> "$RESULTS_FILE"
            
            # Add to markdown summary
            command_text=$(echo "$cmd" | cut -c 1-50)
            if [ ${#command_text} -gt 50 ]; then
                command_text="${command_text}..."
            fi
            echo "| $test_num | $(basename "$test_file") | $line_num | \`$command_text\` | ❌ Failed |" >> "$SUMMARY_FILE"
        else
            echo "Test #$test_num: File not found ($file_path)" >> "$RESULTS_FILE"
            echo "| $test_num | File not found | $line_num | Error: Test file not found | ❓ Unknown |" >> "$SUMMARY_FILE"
        fi
    fi
done < <(grep -E "${TEST_TYPE}:.*❌" "$OUTPUT_FILE")

echo >> "$RESULTS_FILE"
echo "=== Detailed Test Results ===" >> "$RESULTS_FILE"
echo >> "$RESULTS_FILE"

# Second pass to process each test case
for i in "${!test_nums[@]}"; do
    test_num="${test_nums[$i]}"
    test_file="${test_files[$i]}"
    line_num="${test_lines[$i]}"
    cmd="${test_cmds[$i]}"
    file_path="${file_paths[$i]}"
    
    # Create temporary files for outputs
    bash_out=$(mktemp)
    mini_out=$(mktemp)
    bash_err=$(mktemp)
    mini_err=$(mktemp)
    bash_exit=0
    mini_exit=0

    # Run in bash
    bash -c "$cmd" > "$bash_out" 2> "$bash_err" 
    bash_exit=$?

    # Run in minishell (if available)
    if [ -x "$MINISHELL_PATH" ]; then
        echo "$cmd" | "$MINISHELL_PATH" > "$mini_out" 2> "$mini_err"
        mini_exit=$?
    else
        echo "Minishell executable not found at $MINISHELL_PATH" > "$mini_out"
        echo "Error: Minishell not available" > "$mini_err"
        mini_exit=127
    fi

    # Print in the exact format requested
    {
        echo "Test # failed $TEST_TYPE test at $file_path"
        echo "=== Test command ==="
        echo "    "
        echo "    $cmd"
        echo "    "
        echo "BASH OUTPUT:"
        cat "$bash_out"
        echo
        echo "MINISHELL OUTPUT:"
        cat "$mini_out"
        echo
    
        if [ "$TEST_TYPE" = "STD_ERR" ]; then
            echo "BASH STDERR:"
            cat "$bash_err"
            echo
            echo "MINISHELL STDERR:"
            cat "$mini_err"
            echo
        fi
        
        if [ "$TEST_TYPE" = "EXIT_CODE" ]; then
            echo "BASH EXIT CODE: $bash_exit"
            echo "MINISHELL EXIT CODE: $mini_exit"
            echo
        fi
    } >> "$RESULTS_FILE"

    # Clean up temp files
    rm -f "$bash_out" "$mini_out" "$bash_err" "$mini_err"
done

# Add test categories to the summary file
{
    echo
    echo "## Test Categories"
    echo
    echo "### Quote Handling"
    echo "Tests related to handling single and double quotes, nested quotes, and escaped quotes."
    echo
    echo "### Variable Expansion"
    echo "Tests for proper \$VAR expansion in different contexts (quotes, command substitution)."
    echo
    echo "### Command Execution"
    echo "Tests for proper command parsing and execution."
    echo
    echo "### Redirections"
    echo "Tests for input/output redirections (<, >, >>, <<)."
    echo
    echo "### Pipes"
    echo "Tests for proper pipe handling (|)."
    echo
    echo "## How to Fix"
    echo
    echo "1. Examine each failing test carefully"
    echo "2. Compare your minishell output with bash output"
    echo "3. Check the test file to understand expected behavior"
    echo "4. Debug your code with the test case"
    echo
    echo "See [Minishell Documentation](https://github.com/djuarez42/Minishell) for more details."
} >> "$SUMMARY_FILE"

echo "=== Extraction complete ==="
echo
echo "Results saved to:"
echo "- Text file: $RESULTS_FILE"
echo "- Markdown summary: $SUMMARY_FILE"
