#!/bin/bash

echo "=== Testing complex backslash patterns ==="
echo

echo "Test 1: /bin/echo \\\\\\$USER (4 backslashes before $USER)"
USER_VALUE=$(echo $USER)
echo -n "Actual bash output: "
/bin/echo \\\\$USER
echo "Explanation: 4 backslashes → 2 literal backslashes, then variable expansion (\\\\$USER → \\\\$USER → \\${USER_VALUE})"
echo

echo "Test 2: /bin/echo \\\\\\\\\\$USER (5 backslashes before $USER)"
echo -n "Actual bash output: "
/bin/echo \\\\\$USER
echo "Explanation: 5 backslashes → 2 literal backslashes + escaped $ (\\\\\\$USER → \\\\\\$USER → \\\\$USER)"
echo

echo "Test 3: /bin/echo \\\\\\\\\\\\\\\\$USER (8 backslashes before $USER)"
echo -n "Actual bash output: "
/bin/echo \\\\\\\\\$USER
echo "Explanation: 8 backslashes → 4 literal backslashes, then variable expansion (\\\\\\\\$USER → \\\\\\\\$USER → \\\\\\${USER_VALUE})"
echo

echo "Test 4: Long example (32 backslashes before $USER)"
echo -n "Actual bash output: "
/bin/echo \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \$PATH \\$PWD
echo "Explanation: 32 backslashes → 16 literal backslashes, then variable expansion, followed by literal \$PATH and expanded \\$PWD"
