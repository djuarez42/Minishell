#!/bin/bash

# Create a temporary file to capture minishell output
TEMP_OUTPUT=$(mktemp)

# Run echo command with ANSI-C quotes
cd /home/ekakhmad/Minishell
echo "Testing echo \$'USER' (ANSI-C quotes)"
./minishell <<EOF | tee $TEMP_OUTPUT
echo \$'USER'
exit
EOF

# Check if any output was produced (besides the prompt)
echo "Output content analysis:"
grep -v "minishell\$" $TEMP_OUTPUT | grep -v "^$" || echo "No output detected"

# For comparison, run the same in bash
echo -e "\nBash output for comparison:"
bash -c "echo \$'USER'"

# Clean up
rm $TEMP_OUTPUT
