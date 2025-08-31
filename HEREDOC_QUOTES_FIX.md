# Heredoc Mixed Quotes Fix

## Problem
When using heredoc with mixed-quoted delimiters (like `"$US"E"R"`), minishell couldn't properly:
1. Detect that the delimiter had quotes (which should prevent variable expansion in the heredoc content)
2. Match the input line with the delimiter to properly terminate the heredoc

## Solution

### 1. Enhanced Quote Detection in create_redir
- Modified `create_redir` in `src/parser/parser_utils2.c` to check for any quotes (single, double, or mixed) in the heredoc delimiter
- When quotes are detected, we:
  - Set `redir->quoted = true` to prevent variable expansion in heredoc content
  - Create a clean version of the delimiter without quotes for proper matching

### 2. Improved Heredoc Processing
- Enhanced comments in `write_heredoc_lines` in `src/executor/redir_utils2.c` to clearly document the behavior:
  - When `redir->quoted` is true, variables in heredoc content are not expanded
  - The delimiter is compared against the clean version (without quotes) for proper matching

### Test Cases
1. Unquoted delimiter: `cat << DELIM` - variables are expanded
2. Single-quoted delimiter: `cat << 'DELIM'` - variables are not expanded
3. Double-quoted delimiter: `cat << "DELIM"` - variables are not expanded
4. Mixed-quoted delimiter: `cat << "D"ELIM` - variables are not expanded
5. Alternating quotes: `cat << "D"'E'"L"'I'"M"` - variables are not expanded
6. Variable in quotes: `cat << "$TEST_VAR"` - variables are not expanded

All test cases now work correctly, matching bash's behavior.
