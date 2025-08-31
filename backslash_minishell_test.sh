#!/bin/bash

# Create a test script for minishell backslash handling
cat > backslash_minishell_test.sh << 'EOT'
#!/bin/bash

echo "Testing backslash handling in minishell"
echo

echo "Test 1: echo \$USER"
echo "Expected: $USER literally (not expanded)"
./minishell -c "echo \$USER"
echo

echo "Test 2: echo \\\\$USER"
echo "Expected: \\ followed by expanded $USER"
./minishell -c "echo \\\\$USER"
echo

echo "Test 3: echo \\\\\$USER"
echo "Expected: \\\$USER literally"
./minishell -c "echo \\\\\$USER"
echo

echo "Test 4: echo \\\\\\\\$USER"
echo "Expected: \\\\ followed by expanded $USER"
./minishell -c "echo \\\\\\\\$USER"
EOT

chmod +x backslash_minishell_test.sh

echo "=== Test script created ==="
echo "You can run it after making sure your minishell supports the -c flag"
echo "If not, you may need to modify it to use input redirection instead"
