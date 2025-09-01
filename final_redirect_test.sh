#!/bin/bash

echo "=== Direct Minishell Test with Simple Redirection ==="
cd /home/ekakhmad/Minishell

# Create a test input script
cat > test_input.sh << 'EOF'
echo DIRECT_TEST > direct_out.txt
exit
EOF

# Run minishell with the script
echo "Running minishell with test input..."
./minishell < test_input.sh

# Check results
echo "Exit status: $?"
echo "Checking for output file..."
ls -la direct_out.txt || echo "File not found!"
if [ -f direct_out.txt ]; then
  echo "File contents:"
  cat direct_out.txt
fi

# Try with a direct echo command
echo "=== Testing with direct command ==="
echo 'echo ANOTHER_TEST > another_out.txt' | ./minishell

# Check results
echo "Exit status: $?"
echo "Checking for output file..."
ls -la another_out.txt || echo "File not found!"
if [ -f another_out.txt ]; then
  echo "File contents:"
  cat another_out.txt
fi
