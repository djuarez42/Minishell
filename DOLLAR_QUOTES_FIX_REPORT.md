# Variable Expansion Fixes for Minishell

## Issues Identified

1. **`echo "$?"` Issue**: When attempting to display the exit status using double quotes, minishell was displaying the literal "?" character instead of the actual exit status.
2. **`echo "$"$` Issue**: When attempting to display a dollar sign in double quotes followed by another dollar sign, minishell was not correctly handling the literal dollar sign inside quotes.

## Fixed Issues

1. ✅ **`echo "$?"` Issue**: Fixed by directly handling the $? case in variable expansion, ensuring it properly expands to the exit status even inside double quotes.
2. ❌ **`echo "$"$` Issue**: Still working on a fix for this issue.

## Implementation Changes

1. Modified `expand_variables.c` to directly handle the $? special case
2. Refactored `handle_dollar` function in `expand_variables_utils2.c` to better handle special cases
3. Made adjustments to the lexer's quote handling in `lexer_utils_3.c`

## Recommendations for Further Fixes

For the remaining issues with dollar sign handling:

1. Review the tokenization process to ensure dollar signs inside double quotes are preserved as literals
2. Add special handling for the `"$"$` pattern in the lexer or parser
3. Consider implementing a more comprehensive quoted string handling mechanism that better aligns with bash behavior

