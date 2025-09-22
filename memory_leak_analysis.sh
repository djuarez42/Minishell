#!/bin/bash

# Colors for better readability
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Memory Leak Detection for Minishell ===${NC}\n"

# Run a command with Valgrind and analyze leak origins
run_memory_test() {
    local cmd="$1"
    local description="$2"
    
    echo -e "${BLUE}Testing: ${description}${NC}"
    echo -e "Command: ${GREEN}$cmd${NC}\n"
    
    # Run with full valgrind details including origin tracking
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
             --log-file=valgrind_output.txt \
             ./minishell -c "$cmd" > /dev/null 2>&1
    
    # Extract the most relevant parts of the valgrind output
    echo -e "${YELLOW}Memory Leak Summary:${NC}"
    grep -A 1 "definitely lost:" valgrind_output.txt
    
    # Show the origins of leaks if any
    echo -e "\n${YELLOW}Leak Origins:${NC}"
    grep -A 20 "definitely lost:" valgrind_output.txt | grep -A 15 "Leak origins" || echo "No specific leak origins found"
    
    # Show some context around the allocations
    echo -e "\n${YELLOW}Allocation Contexts:${NC}"
    grep -A 20 "allocated at" valgrind_output.txt | head -n 20 || echo "No allocation contexts found"
    
    echo -e "\n${RED}-----------------------------------------------------${NC}\n"
}

# Tests for the specific syntax error cases
run_memory_test "echo > <" "Redirect syntax error with echo > <"
run_memory_test "/bin/cat ><" "Redirect syntax error with /bin/cat ><"
run_memory_test "/bin/cat <Makefile >" "Redirect syntax error with /bin/cat <Makefile >"
run_memory_test "echo >" "Incomplete redirect with echo >"

# Additional tests to help isolate the cause
run_memory_test "echo test" "Simple echo command (for comparison)"
run_memory_test "echo test > testfile" "Valid redirect (for comparison)"
run_memory_test "cat < Makefile" "Valid input redirect (for comparison)"

echo -e "${YELLOW}Memory Testing Complete${NC}"
echo -e "Check valgrind_output.txt for complete details"

# Clean up
rm -f testfile
