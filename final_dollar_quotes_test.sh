#!/bin/bash

echo "=== Testing in bash ==="
echo "1. echo \$\"HOME\":"
/bin/echo $"HOME"
echo "2. echo \"\$HOME\":"
/bin/echo "$HOME"
echo "3. echo \$HOME:"
/bin/echo $HOME
echo "4. echo \$\"HOME\"\$USER:"
/bin/echo $"HOME"$USER
echo "5. echo \$\"\\$USER\":"
/bin/echo $"\$USER"
echo "6. echo \"\$\"HOME:"
/bin/echo "$"HOME

echo -e "\n=== Testing in minishell ==="
./minishell << 'INNER'
echo "1. echo $\"HOME\":"
echo $"HOME"
echo "2. echo \"$HOME\":"
echo "$HOME"
echo "3. echo $HOME:"
echo $HOME
echo "4. echo $\"HOME\"$USER:"
echo $"HOME"$USER
echo "5. echo $\"\\$USER\":"
echo $"\$USER"
echo "6. echo \"$\"HOME:"
echo "$"HOME
exit
INNER
