#!/bin/bash

cd /home/ekakhmad/Minishell
echo "Starting basic test of minishell..."
./minishell << EOF
echo "Hello, World!"
exit
EOF
