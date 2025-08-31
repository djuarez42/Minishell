#!/bin/bash

# Create a direct test for the dollar-dollar case
echo "Creating direct test for the dollar-dollar case..."

# Create a specialized test script that directly modifies the minishell code for debugging
cat > direct_dollar_dollar_test.sh << 'EOF'
#!/bin/bash

# Set up a test file
cat > test_input.txt << 'TEST_END'
/bin/echo "$"$
exit
TEST_END

# Run the minishell with the test input
./minishell < test_input.txt

# Run the actual bash command to see expected output
echo "Expected output from bash:"
/bin/echo "$"$

# Clean up
rm test_input.txt
EOF

# Make it executable
chmod +x direct_dollar_dollar_test.sh

# Run the test
./direct_dollar_dollar_test.sh
