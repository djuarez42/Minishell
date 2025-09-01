#!/bin/bash

cd /home/ekakhmad/Minishell
echo "Starting test of minishell redirection..."
./minishell << EOF
echo "Hello, World!" > test_file.txt
exit
EOF
echo "Checking if test_file.txt was created:"
ls -la test_file.txt 2>/dev/null || echo "File doesn't exist"
