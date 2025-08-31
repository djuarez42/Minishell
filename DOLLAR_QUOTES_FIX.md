# Variable Expansion Fixes for Minishell

## Issue Description

We've identified a critical issue in the minishell implementation where variable expansion in double quotes wasn't working correctly. In particular:

- `echo "$?"` was not properly expanding to the exit status
- `echo "$"$` was not correctly handling the double quotes

The bash-expected behaviors are:
1. `$?` inside double quotes should expand to the exit status
2. `"$"` should be interpreted as a literal `$` (in double quotes)
3. `$` outside quotes should be interpreted as a literal `$`

## Solution

The issue has been fixed by:

1. Refactoring the `handle_dollar` function in `expand_variables_utils2.c` to properly handle special characters following `$`
2. Adding a new file `dollar_quotes_fix.c` with a helper function for better handling of dollar characters inside quotes
3. Modifying `expand_variables.c` to use the new dollar quotes handling function

## Files Changed

1. `src/executor/expand_variables_utils2.c` - Updated `handle_dollar` function
2. `src/executor/expand_variables.c` - Modified to use the new quote handling function
3. Added `src/executor/dollar_quotes_fix.c` - New file with the improved handling logic
4. Updated the Makefile to include the new file

## Testing

Commands that should now work correctly:
- `echo "$?"`
- `echo "$"$`
- All other commands in the variables test suite

## Implementation Details

The key insight was that variable expansion needed to properly handle special characters following `$` (like `?`) both inside and outside of quotes. The `handle_dollar` function was refactored to first check for these special cases before attempting to treat the text as a variable name.
