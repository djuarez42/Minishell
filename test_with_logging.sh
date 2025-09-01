#!/bin/bash

cd /home/ekakhmad/Minishell
echo "Testing redirection with debug logging..."

# Create a test file
echo "Test content" > input.txt

# Enable our logger
export REDIRECTION_DEBUG=1

# Run minishell with redirection
./minishell << EOF
cat < input.txt > output.txt
exit
EOF

# Check results
echo "Checking if output.txt was created:"
ls -la output.txt 2>/dev/null || echo "File doesn't exist"
if [ -f output.txt ]; then
    echo "Contents of output.txt:"
    cat output.txt
fi

# Check if log file was created
echo "Checking for log files:"
ls -la ms_diag.log 2>/dev/null || echo "No ms_diag.log file"
