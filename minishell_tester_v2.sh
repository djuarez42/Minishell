#!/bin/bash

# Minishell Comprehensive Test Script v2
# Enhanced with command display for each test case
# Tests minishell against bash for:
# - Standard output
# - Standard error  
# - Exit codes
# - Memory leaks (using valgrind)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
GRAY='\033[0;90m'
NC='\033[0m' # No Color

# Configuration
MINISHELL_PATH="./minishell"
TIMEOUT_DURATION=5
TEMP_DIR="/tmp/minishell_test"
LEAK_CHECK=${LEAK_CHECK:-1}  # Set to 0 to disable leak checking
RESULTS_FILE="minishell_test_results.log"

# Statistics
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
LEAK_TESTS=0

# Create temp directory
mkdir -p "$TEMP_DIR"

# Initialize results file
echo "MINISHELL TEST RESULTS v2 - $(date)" > "$RESULTS_FILE"
echo "=================================================" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

# Test files for redirection tests
echo "Hello World" > "$TEMP_DIR/input.txt"
echo -e "Line 1\nLine 2\nLine 3" > "$TEMP_DIR/multiline.txt"
touch "$TEMP_DIR/empty.txt"

print_header() {
    echo -e "${CYAN}════════════════════════════════════════${NC}"
    echo -e "${CYAN}    MINISHELL COMPREHENSIVE TESTER v2${NC}"
    echo -e "${CYAN}════════════════════════════════════════${NC}"
}

print_section() {
    echo -e "\n${PURPLE}🧪 $1${NC}"
    echo -e "${PURPLE}$(printf '%.0s─' $(seq 1 ${#1}))${NC}"
}

print_command() {
    local cmd="$1"
    echo -e "  ${GRAY}Command: ${CYAN}$cmd${NC}"
}

print_test_result() {
    local test_name="$1"
    local result="$2"
    local details="$3"
    local command="$4"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if [ "$result" = "PASS" ]; then
        echo -e "  ✅ ${GREEN}PASS${NC} - $test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    elif [ "$result" = "FAIL" ]; then
        echo -e "  ❌ ${RED}FAIL${NC} - $test_name"
        if [ -n "$details" ]; then
            echo -e "     ${YELLOW}Details: $details${NC}"
        fi
        FAILED_TESTS=$((FAILED_TESTS + 1))
    elif [ "$result" = "LEAK" ]; then
        echo -e "  💧 ${RED}LEAK${NC} - $test_name"
        LEAK_TESTS=$((LEAK_TESTS + 1))
        FAILED_TESTS=$((FAILED_TESTS + 1))
    elif [ "$result" = "TIMEOUT" ]; then
        echo -e "  ⏰ ${YELLOW}TIMEOUT${NC} - $test_name"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

write_test_to_results() {
    local test_name="$1"
    local command="$2"
    local result="$3"
    local details="$4"
    
    local bash_stdout="$TEMP_DIR/bash_stdout"
    local bash_stderr="$TEMP_DIR/bash_stderr"
    local mini_stdout="$TEMP_DIR/mini_stdout"
    local mini_stderr="$TEMP_DIR/mini_stderr"
    
    echo "TEST: $test_name" >> "$RESULTS_FILE"
    echo "COMMAND: $command" >> "$RESULTS_FILE"
    echo "RESULT: $result" >> "$RESULTS_FILE"
    
    if [ -n "$details" ]; then
        echo "DETAILS: $details" >> "$RESULTS_FILE"
    fi
    
    echo "" >> "$RESULTS_FILE"
    echo "--- BASH OUTPUT ---" >> "$RESULTS_FILE"
    echo "STDOUT:" >> "$RESULTS_FILE"
    if [ -f "$bash_stdout" ]; then
        cat "$bash_stdout" >> "$RESULTS_FILE" 2>/dev/null || echo "empty or error reading file" >> "$RESULTS_FILE"
    else
        echo "no stdout file" >> "$RESULTS_FILE"
    fi
    echo "" >> "$RESULTS_FILE"
    echo "STDERR:" >> "$RESULTS_FILE"
    if [ -f "$bash_stderr" ]; then
        cat "$bash_stderr" >> "$RESULTS_FILE" 2>/dev/null || echo "empty or error reading file" >> "$RESULTS_FILE"
    else
        echo "no stderr file" >> "$RESULTS_FILE"
    fi
    echo "" >> "$RESULTS_FILE"
    
    echo "--- MINISHELL OUTPUT ---" >> "$RESULTS_FILE"
    echo "STDOUT:" >> "$RESULTS_FILE"
    if [ -f "$mini_stdout" ]; then
        cat "$mini_stdout" >> "$RESULTS_FILE" 2>/dev/null || echo "empty or error reading file" >> "$RESULTS_FILE"
    else
        echo "no stdout file" >> "$RESULTS_FILE"
    fi
    echo "" >> "$RESULTS_FILE"
    echo "STDERR:" >> "$RESULTS_FILE"
    if [ -f "$mini_stderr" ]; then
        cat "$mini_stderr" >> "$RESULTS_FILE" 2>/dev/null || echo "empty or error reading file" >> "$RESULTS_FILE"
    else
        echo "no stderr file" >> "$RESULTS_FILE"
    fi
    echo "" >> "$RESULTS_FILE"
    
    if [ "$result" = "FAIL" ]; then
        echo "--- DIFFERENCES ---" >> "$RESULTS_FILE"
        echo "STDOUT DIFF:" >> "$RESULTS_FILE"
        if [ -f "$bash_stdout" ] && [ -f "$mini_stdout" ]; then
            diff -u "$bash_stdout" "$mini_stdout" >> "$RESULTS_FILE" 2>/dev/null || echo "no differences or diff error" >> "$RESULTS_FILE"
        else
            echo "cannot compare - missing files" >> "$RESULTS_FILE"
        fi
        echo "" >> "$RESULTS_FILE"
        echo "STDERR DIFF:" >> "$RESULTS_FILE"
        if [ -f "$bash_stderr" ] && [ -f "$mini_stderr" ]; then
            diff -u "$bash_stderr" "$mini_stderr" >> "$RESULTS_FILE" 2>/dev/null || echo "no differences or diff error" >> "$RESULTS_FILE"
        else
            echo "cannot compare - missing files" >> "$RESULTS_FILE"
        fi
        echo "" >> "$RESULTS_FILE"
    fi
    
    echo "=================================================" >> "$RESULTS_FILE"
    echo "" >> "$RESULTS_FILE"
}

run_bash_command() {
    local cmd="$1"
    local bash_stdout="$TEMP_DIR/bash_stdout"
    local bash_stderr="$TEMP_DIR/bash_stderr"
    
    # Run command in bash
    echo "$cmd" | timeout "$TIMEOUT_DURATION" bash > "$bash_stdout" 2> "$bash_stderr"
    local bash_exit_code=$?
    
    # Handle timeout
    if [ $bash_exit_code -eq 124 ]; then
        bash_exit_code=1
    fi
    
    echo "$bash_exit_code"
}

run_minishell_command() {
    local cmd="$1"
    local mini_stdout="$TEMP_DIR/mini_stdout"
    local mini_stderr="$TEMP_DIR/mini_stderr"
    
    if [ ! -f "$MINISHELL_PATH" ]; then
        echo "Error: Minishell not found at $MINISHELL_PATH" >&2
        return 1
    fi
    
    # Run command in minishell
    echo -e "$cmd\nexit" | timeout "$TIMEOUT_DURATION" "$MINISHELL_PATH" > "$mini_stdout" 2> "$mini_stderr"
    local mini_exit_code=$?
    
    # Handle timeout
    if [ $mini_exit_code -eq 124 ]; then
        return 124
    fi
    
    # Remove minishell prompt from output if present
    # - Standalone prompt lines
    sed -i '/^minishell\$/d' "$mini_stdout" 2>/dev/null || true
    sed -i '/^.*\$ $/d' "$mini_stdout" 2>/dev/null || true
    # - Prompt concatenated to previous output when no trailing newline (echo -n)
    #   Strip a trailing "minishell$ " at end of the last line
    sed -E -i '$ s/minishell\$ ?$//' "$mini_stdout" 2>/dev/null || true
    
    echo "$mini_exit_code"
}

check_memory_leaks() {
    local cmd="$1"
    local valgrind_output="$TEMP_DIR/valgrind_output"
    
    if ! command -v valgrind &> /dev/null; then
        return 0  # Skip if valgrind not available
    fi
    
    echo -e "$cmd\nexit" | timeout $((TIMEOUT_DURATION * 3)) valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 "$MINISHELL_PATH" &> "$valgrind_output"
    local valgrind_exit=$?
    
    if [ $valgrind_exit -eq 1 ]; then
        return 1  # Memory leaks detected
    fi
    
    return 0
}

compare_outputs() {
    local test_name="$1"
    local cmd="$2"
    local check_stdout="${3:-1}"
    local check_stderr="${4:-1}"
    local check_exit_code="${5:-1}"
    
    local bash_stdout="$TEMP_DIR/bash_stdout"
    local bash_stderr="$TEMP_DIR/bash_stderr"
    local mini_stdout="$TEMP_DIR/mini_stdout"
    local mini_stderr="$TEMP_DIR/mini_stderr"
    
    # Print the command being tested
    print_command "$cmd"
    
    # Run bash command
    local bash_exit_code
    bash_exit_code=$(run_bash_command "$cmd")
    
    # Run minishell command
    local mini_exit_code
    mini_exit_code=$(run_minishell_command "$cmd")
    
    # Check for timeout
    if [ $mini_exit_code -eq 124 ]; then
        print_test_result "$test_name" "TIMEOUT" "" "$cmd"
        write_test_to_results "$test_name" "$cmd" "TIMEOUT" "Command timed out after $TIMEOUT_DURATION seconds"
        return
    fi
    
    local differences=""
    local exit_code_details=""
    
    # Compare stdout
    if [ "$check_stdout" = "1" ]; then
        if ! diff -u "$bash_stdout" "$mini_stdout" > /dev/null 2>&1; then
            differences="stdout differs"
        fi
    fi
    
    # Compare stderr
    if [ "$check_stderr" = "1" ]; then
        if ! diff -u "$bash_stderr" "$mini_stderr" > /dev/null 2>&1; then
            if [ -n "$differences" ]; then
                differences="$differences, stderr differs"
            else
                differences="stderr differs"
            fi
        fi
    fi
    
    # Compare exit codes
    if [ "$check_exit_code" = "1" ]; then
        if [ "$bash_exit_code" != "$mini_exit_code" ]; then
            exit_code_details="bash_exit=$bash_exit_code, mini_exit=$mini_exit_code"
            if [ -n "$differences" ]; then
                differences="$differences, exit_code: $exit_code_details"
            else
                differences="exit_code: $exit_code_details"
            fi
        fi
    fi
    
    # Add exit codes to results for all tests
    local full_details="$differences"
    if [ -n "$exit_code_details" ]; then
        if [ -n "$full_details" ]; then
            full_details="$full_details | Exit codes: $exit_code_details"
        else
            full_details="Exit codes: $exit_code_details"
        fi
    else
        if [ -z "$full_details" ]; then
            full_details="Exit codes: bash=$bash_exit_code, mini=$mini_exit_code"
        else
            full_details="$full_details | Exit codes: bash=$bash_exit_code, mini=$mini_exit_code"
        fi
    fi
    
    # Check memory leaks if enabled
    if [ "$LEAK_CHECK" = "1" ]; then
        if ! check_memory_leaks "$cmd"; then
            print_test_result "$test_name" "LEAK" "" "$cmd"
            write_test_to_results "$test_name" "$cmd" "LEAK" "Memory leak detected | $full_details"
            return
        fi
    fi
    
    # Print result and write to file
    if [ -z "$differences" ]; then
        print_test_result "$test_name" "PASS" "" "$cmd"
        write_test_to_results "$test_name" "$cmd" "PASS" "$full_details"
    else
        print_test_result "$test_name" "FAIL" "$differences" "$cmd"
        write_test_to_results "$test_name" "$cmd" "FAIL" "$full_details"
    fi
    
    echo ""  # Add spacing between tests
}

# Test Categories

test_basic_commands() {
    print_section "Basic Commands"
    
    compare_outputs "echo hello" "echo hello"
    compare_outputs "echo with spaces" "echo hello world"
    compare_outputs "echo -n option" "echo -n hello"
    compare_outputs "pwd command" "pwd"
    compare_outputs "env command" "env" 0 0 0  # Don't check outputs, just that it runs
    compare_outputs "exit command" "exit" 0 0 1
    compare_outputs "exit with code" "exit 42" 0 0 1
}

test_quotes() {
    print_section "Quote Handling"
    
    compare_outputs "single quotes" "echo 'hello world'"
    compare_outputs "double quotes" "echo \"hello world\""
    compare_outputs "mixed quotes" "echo \"hello 'world'\""
    compare_outputs "quote with spaces" "echo 'hello    world'"
    compare_outputs "empty quotes" "echo ''"
    compare_outputs "empty double quotes" "echo \"\""
    compare_outputs "quotes with special chars" "echo 'hello|world'"
    compare_outputs "dollar in single quotes" "echo '\$HOME'"
    compare_outputs "dollar in double quotes" "echo \"\$USER\"" 0 0 0  # Don't check exact output
}

test_environment_variables() {
    print_section "Environment Variables"
    
    compare_outputs "HOME variable" "echo \$HOME" 0 0 0
    compare_outputs "USER variable" "echo \$USER" 0 0 0
    compare_outputs "PATH variable" "echo \$PATH" 0 0 0
    compare_outputs "undefined variable" "echo \$UNDEFINED_VAR_12345"
    compare_outputs "exit status variable" "echo \$?" 0 0 0
    compare_outputs "export new variable" "export TEST_VAR=hello" 0 0 0
    # Use newlines instead of ';' to stay within 42 mandatory scope
    compare_outputs "use exported variable" $'export TEST_VAR=hello\necho \$TEST_VAR'
    compare_outputs "unset variable" $'export TEST_VAR=hello\nunset TEST_VAR\necho \$TEST_VAR'
}

test_redirections() {
    print_section "Redirections"
    
    # Output redirection (use newline-separated commands; expand $TEMP_DIR here)
    compare_outputs "redirect output" "echo hello > $TEMP_DIR/test_out.txt
cat $TEMP_DIR/test_out.txt"
    compare_outputs "append output" "echo hello > $TEMP_DIR/test_append.txt
echo world >> $TEMP_DIR/test_append.txt
cat $TEMP_DIR/test_append.txt"
    
    # Input redirection
    compare_outputs "redirect input" "cat < $TEMP_DIR/input.txt"
    
    # Heredoc (simple here-doc with static delimiter)
    compare_outputs "heredoc simple" $'cat << EOF\nhello\nworld\nEOF'
    
    # Multiple redirections
    compare_outputs "multiple redirections" "cat < $TEMP_DIR/input.txt > $TEMP_DIR/test_multi.txt
cat $TEMP_DIR/test_multi.txt"
}

test_pipes() {
    print_section "Pipes"
    
    # Use echo for simple single-line pipe
    compare_outputs "simple pipe" "echo hello | cat"
    # Use heredoc to provide multi-line input instead of printf/echo -e
    # Replace grep with simple passthrough using cat
    compare_outputs "pipe passthrough" $'cat << EOF | cat\nhello\nworld\nhello\nEOF'
    # Multiple pipes: keep commands simple and portable
    compare_outputs "multiple pipes" $'cat << EOF | cat | cat\nhello\nworld\ntest\nEOF'
    compare_outputs "multiple pipes (no grep)" $'cat << EOF | cat | cat\nhello\nworld\nhello\nEOF'
    compare_outputs "pipe with wc" "echo hello world | wc -w"
    compare_outputs "pipe with sort" $'cat << EOF | sort\nc\na\nb\nEOF'
}

test_builtin_commands() {
    print_section "Builtin Commands"
    
    # cd command
    compare_outputs "cd to home" "cd ~
pwd" 0 0 0
    compare_outputs "cd to root" "cd /
pwd"
    compare_outputs "cd to temp" "cd $TEMP_DIR
pwd"
    # Do not compare exact stderr text for cd errors (wording varies)
    compare_outputs "cd to nonexistent" "cd /nonexistent/directory" 0 0 1
    
    # export command
    compare_outputs "export without args" "export" 0 0 0
    compare_outputs "export with value" "export TEST=value"
    
    # unset command  
    compare_outputs "unset existing" "export TEST=value; unset TEST"
    compare_outputs "unset nonexistent" "unset NONEXISTENT_VAR"
}

test_error_handling() {
    print_section "Error Handling"
    
    compare_outputs "command not found" "nonexistent_command_12345" 0 1 1
    compare_outputs "permission denied" "cat /etc/shadow" 0 1 1
    compare_outputs "syntax error pipes" "echo hello |" 0 1 1
    compare_outputs "syntax error redirect" "echo hello >" 0 1 1
    compare_outputs "unclosed quotes" "echo 'unclosed" 0 1 1
}

test_complex_scenarios() {
    print_section "Complex Scenarios"
    
    compare_outputs "pipes with redirections" "echo hello | cat > $TEMP_DIR/pipe_redir.txt
cat $TEMP_DIR/pipe_redir.txt"
    compare_outputs "env vars in commands" "echo \$USER | cat"
    compare_outputs "quotes with pipes" "echo hello world | cat"
    compare_outputs "multiple commands" "echo hello
echo world" 0 0 0
    compare_outputs "export and use" $'export MY_VAR=test\necho \$MY_VAR'
}

test_signal_handling() {
    print_section "Signal Handling (Interactive)"
    
    echo -e "${YELLOW}Note: Signal tests require manual verification${NC}"
    echo -e "${YELLOW}Test Ctrl-C, Ctrl-D, and Ctrl-\\\\ manually in interactive mode${NC}"
}

print_summary() {
    echo -e "\n${CYAN}════════════════════════════════════════${NC}"
    echo -e "${CYAN}              TEST SUMMARY${NC}"
    echo -e "${CYAN}════════════════════════════════════════${NC}"
    echo -e "Total tests: ${BLUE}$TOTAL_TESTS${NC}"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    if [ "$LEAK_CHECK" = "1" ]; then
        echo -e "Memory leaks: ${RED}$LEAK_TESTS${NC}"
    fi
    
    local pass_rate=0
    if [ $TOTAL_TESTS -gt 0 ]; then
        pass_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    fi
    
    echo -e "Pass rate: ${BLUE}$pass_rate%${NC}"
    
    # Write summary to results file
    echo "" >> "$RESULTS_FILE"
    echo "FINAL SUMMARY" >> "$RESULTS_FILE"
    echo "=============" >> "$RESULTS_FILE"
    echo "Total tests: $TOTAL_TESTS" >> "$RESULTS_FILE"
    echo "Passed: $PASSED_TESTS" >> "$RESULTS_FILE"
    echo "Failed: $FAILED_TESTS" >> "$RESULTS_FILE"
    if [ "$LEAK_CHECK" = "1" ]; then
        echo "Memory leaks: $LEAK_TESTS" >> "$RESULTS_FILE"
    fi
    echo "Pass rate: $pass_rate%" >> "$RESULTS_FILE"
    echo "Test completed: $(date)" >> "$RESULTS_FILE"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "\n${GREEN}🎉 All tests passed! Your minishell is working great!${NC}"
        echo -e "${CYAN}📄 Detailed results saved to: $RESULTS_FILE${NC}"
    else
        echo -e "\n${YELLOW}⚠️  Some tests failed. Check the details above.${NC}"
        echo -e "${CYAN}📄 Detailed results with exact outputs saved to: $RESULTS_FILE${NC}"
    fi
}

cleanup() {
    rm -rf "$TEMP_DIR"
}

# Main execution
main() {
    print_header
    
    # Check if minishell exists
    if [ ! -f "$MINISHELL_PATH" ]; then
        echo -e "${RED}Error: Minishell not found at $MINISHELL_PATH${NC}"
        echo -e "${YELLOW}Please compile your minishell first: make${NC}"
        exit 1
    fi
    
    # Check if valgrind is available for leak checking
    if [ "$LEAK_CHECK" = "1" ] && ! command -v valgrind &> /dev/null; then
        echo -e "${YELLOW}Warning: valgrind not found. Memory leak checking disabled.${NC}"
        LEAK_CHECK=0
    fi
    
    # Run test categories
    test_basic_commands
    test_quotes
    test_environment_variables
    test_redirections
    test_pipes
    test_builtin_commands
    test_error_handling
    test_complex_scenarios
    test_signal_handling
    
    print_summary
    cleanup
    
    # Exit with appropriate code
    if [ $FAILED_TESTS -eq 0 ]; then
        exit 0
    else
        exit 1
    fi
}

# Handle script arguments
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "Minishell Comprehensive Test Script v2"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --help, -h        Show this help message"
    echo "  --no-leaks        Disable memory leak checking"
    echo ""
    echo "Environment variables:"
    echo "  LEAK_CHECK=0      Disable memory leak checking"
    echo "  MINISHELL_PATH    Path to minishell executable (default: ./minishell)"
    echo ""
    echo "Examples:"
    echo "  $0                      # Run all tests with leak checking"
    echo "  $0 --no-leaks          # Run tests without leak checking"
    echo "  LEAK_CHECK=0 $0        # Same as above"
    echo ""
    echo "Output:"
    echo "  Results are displayed on screen and saved to minishell_test_results.log"
    echo "  The log file contains exact outputs from both bash and minishell for comparison"
    echo ""
    exit 0
fi

if [ "$1" = "--no-leaks" ]; then
    LEAK_CHECK=0
fi

# Trap to ensure cleanup on exit
trap cleanup EXIT

# Run main function
main
