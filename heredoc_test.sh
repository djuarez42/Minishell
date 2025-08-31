#!/bin/bash

export TEST_VAR="test_value"

# Test mixed quotes in heredoc delimiter
cat << "DE"LI"M"
This is a heredoc with $TEST_VAR
The variable should not be expanded
"DE"LI"M"

echo "Test completed"
