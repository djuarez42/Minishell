#!/bin/bash
set -x
cd /home/ekakhmad/Minishell
./minishell << 'EOF'
echo DIRECT_TEST > direct_stdout_test.txt
EOF
echo "Exit status: $?"
ls -la direct_stdout_test.txt
cat direct_stdout_test.txt
