#!/bin/bash

# Comprehensive Test Suite for Contributor A and B Plans
# Tests tokenizer, parser, redirections, expansions, and edge cases

echo "🧪 COMPREHENSIVE TEST SUITE - ALL CONTRIBUTOR A & B CASES"
echo "========================================================="

# Set up test environment
export USER="ekaterina"
export HOME="/Users/ekaterina"
export TESTVAR="testvalue"

# Test counter
PASSED=0
FAILED=0
TOTAL=0

# Helper function to run test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected="$3"
    local description="$4"
    
    TOTAL=$((TOTAL + 1))
    echo
    echo "Test $TOTAL: $test_name"
    echo "Command: $command"
    echo "Expected: $expected"
    echo "Description: $description"
    
    # Run the command and capture output
    actual=$(echo "$command" | ./minishell 2>/dev/null)
    
    if [ "$actual" = "$expected" ]; then
        echo "✅ PASSED"
        PASSED=$((PASSED + 1))
    else
        echo "❌ FAILED"
        echo "Actual: '$actual'"
        FAILED=$((FAILED + 1))
    fi
}

echo
echo "🔧 Building minishell..."
make -j4 > /dev/null 2>&1

echo
echo "📋 CONTRIBUTOR A TESTS - Tokenizer, Parser, Redirections"
echo "======================================================="

# A1: Basic tokenization without spaces
run_test "A1-Tokenization" \
    "echo hello|cat" \
    "hello" \
    "Pipe without spaces should tokenize correctly"

# A2: Operator adjacency
run_test "A2-Operator-Adjacency" \
    "echo test>output.txt && cat output.txt && rm output.txt" \
    "test" \
    "Redirection without spaces should work"

# A3: Multiple operators
run_test "A3-Multiple-Operators" \
    "echo hello>temp.txt<input.txt 2>/dev/null || echo 'Complex operators'" \
    "Complex operators" \
    "Multiple adjacent operators should parse"

# A4: Basic redirection
run_test "A4-Basic-Redirection" \
    "echo 'redirection test' > /tmp/redir_test.txt && cat /tmp/redir_test.txt && rm /tmp/redir_test.txt" \
    "redirection test" \
    "Basic output redirection should work"

# A5: Input redirection
run_test "A5-Input-Redirection" \
    "echo 'input test' > /tmp/input.txt && cat < /tmp/input.txt && rm /tmp/input.txt" \
    "input test" \
    "Input redirection should work"

# A6: Append redirection
run_test "A6-Append-Redirection" \
    "echo 'line1' > /tmp/append.txt && echo 'line2' >> /tmp/append.txt && cat /tmp/append.txt && rm /tmp/append.txt" \
    $'line1\nline2' \
    "Append redirection should work"

# A7: Quoted filenames
run_test "A7-Quoted-Filenames" \
    "echo 'quoted file test' > '/tmp/quoted file.txt' && cat '/tmp/quoted file.txt' && rm '/tmp/quoted file.txt'" \
    "quoted file test" \
    "Quoted filenames should work"

# A8: Syntax error - leading pipe
run_test "A8-Syntax-Error-Leading-Pipe" \
    "| echo hello" \
    "" \
    "Leading pipe should be syntax error (empty output)"

# A9: Syntax error - trailing pipe
run_test "A9-Syntax-Error-Trailing-Pipe" \
    "echo hello |" \
    "" \
    "Trailing pipe should be syntax error"

# A10: Syntax error - double pipe
run_test "A10-Syntax-Error-Double-Pipe" \
    "echo hello || echo world" \
    "" \
    "Double pipe should be syntax error in minishell"

echo
echo "📋 CONTRIBUTOR B TESTS - Variable Expansion & Tilde"
echo "=================================================="

# B1: Remove $USER heuristic - normal expansion
run_test "B1-Normal-USER-Expansion" \
    "echo \$USER" \
    "ekaterina" \
    "Normal \$USER expansion should work"

# B2: Trailing $ edge case
run_test "B2-Trailing-Dollar" \
    "echo \$" \
    "" \
    "Trailing \$ should output empty string"

# B3: $" edge case - $ disappears
run_test "B3-Dollar-Quote-Edge" \
    "echo \$\"HOME\"" \
    "HOME" \
    "\$\" should make \$ disappear, leaving quoted content"

# B4: $' edge case - $ disappears  
run_test "B4-Dollar-Single-Quote" \
    "echo \$'HOME'" \
    "HOME" \
    "\$' should make \$ disappear, leaving quoted content"

# B5: Complex $" case with variable
run_test "B5-Complex-Dollar-Quote" \
    "echo \$\"HOME\"\$USER" \
    "HOMEekaterina" \
    "Complex \$\"HOME\"\$USER should output HOMEekaterina"

# B6: $<digit> support
run_test "B6-Dollar-Digit" \
    "echo \$1 \$9" \
    " " \
    "\$1 and \$9 should expand (likely empty)"

# B7: Invalid variable name
run_test "B7-Invalid-Var-Name" \
    "echo \$@invalid" \
    "\$@invalid" \
    "Invalid variable names should remain literal"

# B8: Tilde expansion - home
run_test "B8-Tilde-Home" \
    "echo ~" \
    "/Users/ekaterina" \
    "~ should expand to \$HOME"

# B9: Tilde expansion - path
run_test "B9-Tilde-Path" \
    "echo ~/Documents" \
    "/Users/ekaterina/Documents" \
    "~/path should expand to \$HOME/path"

# B10: Tilde in middle (no expansion)
run_test "B10-Tilde-Middle" \
    "echo test~file" \
    "test~file" \
    "~ in middle should not expand"

# B11: Multiple quotes with $USER
run_test "B11-Multiple-Quotes" \
    "echo \"\"\"\"\"\"\"\"\$USER\"\"\"\"\"\"\"\"" \
    "ekaterina" \
    "Multiple quotes with \$USER should expand"

# B12: Mixed quotes
run_test "B12-Mixed-Quotes" \
    "echo '\"' \$USER\"'\"" \
    "\" ekaterina'" \
    "Mixed quotes should produce literal quotes around expanded content"

# B13: Single quotes suppress expansion
run_test "B13-Single-Quote-Suppress" \
    "echo '\$USER'" \
    "\$USER" \
    "Single quotes should suppress expansion"

# B14: Double quotes allow expansion
run_test "B14-Double-Quote-Expand" \
    "echo \"\$USER\"" \
    "ekaterina" \
    "Double quotes should allow expansion"

# B15: Empty quote continuation (known issue)
run_test "B15-Empty-Quote-Continue" \
    "echo \$\"\"\$USER" \
    "ekaterina" \
    "Empty quotes should not break continuation"

echo
echo "📋 COMPLEX INTEGRATION TESTS"
echo "============================"

# I1: Complex redirection with expansion
run_test "I1-Complex-Redir-Expand" \
    "echo \$USER > /tmp/\$USER.txt && cat /tmp/\$USER.txt && rm /tmp/\$USER.txt" \
    "ekaterina" \
    "Redirection with variable expansion in filename"

# I2: Pipe with expansion
run_test "I2-Pipe-Expansion" \
    "echo \$USER | cat" \
    "ekaterina" \
    "Pipe with variable expansion"

# I3: Tilde in redirection
run_test "I3-Tilde-Redirection" \
    "echo 'tilde test' > ~/tilde_test.tmp && cat ~/tilde_test.tmp && rm ~/tilde_test.tmp" \
    "tilde test" \
    "Tilde expansion in redirection filename"

# I4: Complex tokenization and expansion
run_test "I4-Complex-Token-Expand" \
    "echo \$USER'test'\$HOME|head -c 20" \
    "ekaterina'test'/Use" \
    "Complex tokenization with mixed quotes and expansion"

# I5: Exit status expansion
run_test "I5-Exit-Status" \
    "false; echo \$?" \
    "1" \
    "Exit status expansion should work"

echo
echo "📊 TEST RESULTS SUMMARY"
echo "======================="
echo "Total Tests: $TOTAL"
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo "Success Rate: $(( PASSED * 100 / TOTAL ))%"

if [ $FAILED -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED! Both Contributor A and B implementations are working correctly."
else
    echo "⚠️  $FAILED tests failed. Review the failing cases above."
fi

echo
echo "🔍 Key Areas Tested:"
echo "- ✓ Tokenizer operator splitting (A)"
echo "- ✓ Parser syntax validation (A)" 
echo "- ✓ Redirection handling (A)"
echo "- ✓ Variable expansion edge cases (B)"
echo "- ✓ Tilde expansion (B)"
echo "- ✓ Quote handling (A+B)"
echo "- ✓ Integration scenarios (A+B)"

exit $FAILED
