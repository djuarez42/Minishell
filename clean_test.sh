#!/bin/bash

# Clear previous files
rm -f heredoc_test.sh

# Create a test script for simple heredoc with mixed quotes
cat > heredoc_test.sh << 'EOF'
#!/bin/bash

export TEST_VAR="test_value"

# Test mixed quotes in heredoc delimiter
cat << "DE"LI"M"
This is a heredoc with $TEST_VAR
The variable should not be expanded
"DE"LI"M"

echo "Test completed"
EOF

chmod +x heredoc_test.sh

# Run in minishell
echo "=== Testing in minishell ==="
./minishell -c "bash heredoc_test.sh"

# Run in bash for comparison
echo -e "\n=== Testing in bash for comparison ==="
bash heredoc_test.sh

echo -e "\n=== Test complete ==="
