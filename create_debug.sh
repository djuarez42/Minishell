#!/bin/bash

# Debug script to help trace variable expansion issues
# Creates a small test file that isolates the mini_test variable behavior

echo "Creating test file..."
cat > debug_mini_test.sh << 'EOF'
/bin/echo $mini_test
env | grep mini_test
export mini_test=
/bin/echo $mini_test
env | grep mini_test
EOF

echo "Making script executable..."
chmod +x debug_mini_test.sh

echo "Adding debug prints to expand_variables.c..."
echo "Compiling with debug changes..."
make re

echo "Ready to test. Run ./minishell then use the debug_mini_test.sh file:"
echo "Run: ./debug_mini_test.sh"
echo "Look for DEBUG: prefixed output to trace variable handling"
