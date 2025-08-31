#!/bin/bash

# Simple test for mixed quotes in heredoc delimiter
echo "Testing mixed quoted delimiter in minishell..."

./minishell << 'EOF'
cat << "$US"E"R"
$USER
Variables should not be expanded because delimiter has quotes
"$US"E"R"
exit
EOF

echo -e "\nTesting unquoted delimiter in minishell..."

./minishell << 'EOF'
cat << DELIM
$USER
Variables should be expanded because delimiter has no quotes
DELIM
exit
EOF
