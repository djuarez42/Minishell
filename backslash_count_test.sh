#!/bin/bash

echo "=== Backslash Counting Test ==="
echo "This test uses echo -E to show the exact number of backslashes"
echo

echo "Test 1: echo -E with 4 backslashes + \$USER"
echo -E "\\\\$USER"
echo "COUNT: 2 backslashes + expanded USER variable"
echo

echo "Test 2: echo -E with 5 backslashes + \$USER"
echo -E "\\\\\$USER"
echo "COUNT: 2 backslashes + literal \$USER"
echo

echo "Test 3: echo -E with 8 backslashes + \$USER"
echo -E "\\\\\\\\$USER"
echo "COUNT: 4 backslashes + expanded USER variable"
echo

echo "Test 4: echo -E with 32 backslashes + \$USER"
echo -E "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER"
echo "COUNT: 16 backslashes + expanded USER variable"
echo

echo "=== BASH Parsing Rule ==="
echo "Backslashes are processed in pairs:"
echo "- Each pair of backslashes (\\\\) becomes a single backslash (\\)"
echo "- If there's an odd number of backslashes before $, the last one escapes the $"
echo "- If there's an even number of backslashes before $, the $ is interpreted for variable expansion"
