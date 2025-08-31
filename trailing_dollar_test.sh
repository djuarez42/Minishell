#!/bin/bash

echo "Testing in BASH:"
echo "echo \$USER\$"
echo $USER$
echo "---"

echo -e "\nTesting in minishell:"
echo "echo \$USER\$" | ./minishell
echo "---"
