#!/bin/bash

# Final test script for the $"string" locale expansion

echo "=== Testing bash behavior ==="
echo "1. echo \$\"HOME\":"
/bin/echo $"HOME"
echo "2. echo \$\"USER\":"
/bin/echo $"USER"
echo "3. echo \$\"HOME\"\$USER:"
/bin/echo $"HOME"$USER
echo "4. echo \$\"\\$USER\":"
/bin/echo $"\$USER"
echo "5. echo \$\"HOME is \$HOME\":"
/bin/echo $"HOME is $HOME"

echo -e "\n=== Testing minishell behavior ==="
./minishell << 'EOF'
echo "1. echo $\"HOME\":"
echo $"HOME"
echo "2. echo $\"USER\":"
echo $"USER"
echo "3. echo $\"HOME\"$USER:"
echo $"HOME"$USER
echo "4. echo $\"\\$USER\":"
echo $"\$USER"
echo "5. echo $\"HOME is \$HOME\":"
echo $"HOME is $HOME"
exit
EOF
