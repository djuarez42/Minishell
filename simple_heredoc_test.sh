#!/bin/bash

# Create test script
cat > test.sh << 'EOF'
#!/bin/bash
export TEST_VAR="test_value"
cat << 'EOF'
$TEST_VAR
EOF
EOF

# Make the script executable
chmod +x test.sh

# Run in minishell
echo "=== Running in minishell ==="
echo './test.sh' | ./minishell

# Run in bash
echo -e "\n=== Running in bash ==="
./test.sh
