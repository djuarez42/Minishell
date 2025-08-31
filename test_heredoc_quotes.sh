#!/bin/bash

# Test script for heredoc with quoted delimiters
echo "Testing heredoc with quoted delimiters in minishell..."

# Create a test script
cat > test_heredoc.sh << 'EOF'
#!/bin/bash

# Test 1: Simple quoted delimiter
echo "Test 1: Simple quoted delimiter"
cat << 'DELIM'
$USER
Variables should not be expanded
DELIM

# Test 2: Double quoted delimiter
echo "Test 2: Double quoted delimiter"
cat << "DELIM"
$USER
Variables should not be expanded
DELIM

# Test 3: Mixed quotes in delimiter
echo "Test 3: Mixed quotes in delimiter"
cat << "$US"E"R"
$USER
Variables should not be expanded
"$US"E"R"

# Test 4: Unquoted delimiter
echo "Test 4: Unquoted delimiter"
cat << DELIM
$USER
Variables should be expanded
DELIM

echo "All tests completed"
EOF

chmod +x test_heredoc.sh

# Run the test in minishell
echo "Running tests in minishell:"
./minishell < test_heredoc.sh

# Run the test in bash for comparison
echo -e "\nRunning tests in bash for comparison:"
bash test_heredoc.sh

# Clean up
rm test_heredoc.sh
