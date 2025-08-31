/* Variable Expansion Fix Plan - Phase 1

1. Function: skip_variable_name in expand_variables_utils.c
   - The issue: Currently this allows alphanumeric characters to be part of a variable name,
     but it doesn't properly detect where the variable name ends.
   - The fix: Ensure it follows POSIX rules for variable names.

2. Function: handle_dollar in expand_variables_utils2.c
   - The issue: May not be handling variable boundaries correctly
   - The fix: Make sure it properly extracts just the valid variable name

3. Function: expand_env_var in expand_variables_utils.c
   - The issue: Simple lookup and expansion without special handling
   - The fix: No changes needed if the variable name is extracted correctly
*/

// Notes on specific test cases:
// 1. $'HOM'E$USER
//    - This is a complex case: $'HOM' is an ANSI-C quoted string, not a variable
//    - The 'E$USER' part should be treated as a literal 'E' followed by the expansion of $USER

// 2. $HALLO42ho
//    - This should look for a variable named exactly "HALLO42ho"
//    - It should not try to match "HALLO42" as a partial match

// 3. $"HOM"E$USER
//    - Similar to case 1, this is a localization string $"HOM" followed by E$USER
