#!/bin/bash

echo "=== Direct Input Test ==="
cd /home/ekakhmad/Minishell

# Create input file
cat > input_commands.txt << 'EOF'
echo FINAL_TEST > final_output.txt
exit
EOF

# Run minishell with input file
./minishell < input_commands.txt

# Check results
echo "Checking for output file..."
ls -la final_output.txt || echo "File not found!"
if [ -f final_output.txt ]; then
  echo "File contents:"
  cat final_output.txt
fi
