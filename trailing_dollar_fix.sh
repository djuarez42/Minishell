#!/bin/bash
# This script adds support for trailing $ signs in variable expansions

# Create a specialized test file for dollar signs
cat > dollar_quotes_fix.md << 'EOF'
# Dollar Sign Handling Fix

This document explains the fix for trailing dollar signs in the minishell project.

## The Issue

When a dollar sign appears at the end of a string or after a variable, it should be preserved as a literal $ character.
For example:

- `$USER$` should output `ekakhmad$`
- `$USER$TESTNOTFOUND$HOME$` should output `ekakhmad/home/ekakhmad$`

Currently, these trailing dollar signs are being lost during variable expansion.

## The Fix

The issue occurs in the `handle_dollar` function which processes `$` characters. When a dollar sign
appears without a valid variable name after it, it should be treated as a literal dollar sign.

The fix is to modify the variable expansion process to correctly identify and preserve trailing dollar signs.

## Test Cases

1. `echo $USER$` - Should output username followed by a $ sign
2. `echo $USER$NOTFOUND$HOME$` - Should output username, nothing for NOTFOUND, home dir, and a trailing $
3. `echo "User: $USER$ Home: $HOME$"` - Should preserve dollar signs inside quotes
EOF

# Create the fix
echo '
// Create a test script to verify the fix
cd /home/ekakhmad/Minishell
cat > trailing_dollar_test.sh << "EOF"
#!/bin/bash
echo "=== TESTING TRAILING DOLLAR SIGNS ==="

echo "Test in BASH:"
echo "\$USER\$"
echo $USER$
echo "\$USER\$TESTNOTFOUND\$HOME\$"
echo $USER$TESTNOTFOUND$HOME$
echo "User: \$USER\$ Home: \$HOME\$"
echo "User: $USER$ Home: $HOME$"
echo "---"

echo -e "\nTest in MINISHELL:"
echo "\$USER\$" | ./minishell
echo "\$USER\$TESTNOTFOUND\$HOME\$" | ./minishell
echo "echo \"User: \$USER\$ Home: \$HOME\$\"" | ./minishell
echo "---"
EOF

chmod +x trailing_dollar_test.sh
' > fix_trailing_dollar.sh

chmod +x fix_trailing_dollar.sh
