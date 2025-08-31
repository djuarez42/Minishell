#!/bin/bash
# Test script for variable persistence issue

echo "Creating test commands..."
cat > test_var_persistence.txt << 'EOF'
/bin/echo $mini_test
export mini_test=hello
/bin/echo $mini_test
unset mini_test
/bin/echo $mini_test
EOF

echo "Running test in minishell..."
./minishell < test_var_persistence.txt
