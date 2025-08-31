#!/bin/bash

# More targeted test for locale string handling

echo "=== Direct testing of locale string handling ==="

./minishell << 'EOF'
echo "Starting test..."
echo $"HOME"
echo "Simple test done."
echo $"USER"
echo "Second test done."
echo $"HOME"$USER
echo "Combined test done."
exit
EOF
