# Variable Expansion Implementation Guide

## 1. Handling ANSI-C Quoting ($'...')

When Bash encounters `$'string'`, it interprets backslash-escaped characters in the string according to ANSI-C rules. This is completely different from variable expansion.

### Implementation Approach:

1. In the `expand_variables` function, when we encounter a `$` followed by a single quote:
   ```c
   if (input[i] == '$' && input[i+1] == '\'') {
       // This is ANSI-C quoting, not a variable
       process_ansi_c_quote(input, &i, &result);
   }
   ```

2. Create a new function `process_ansi_c_quote` that:
   - Extracts the content between the quotes
   - Processes any backslash-escaped characters
   - Appends the result to the output string
   - Updates the position index

3. Example: For `$'HOM'E$USER`:
   - `$'HOM'` should be processed as an ANSI-C quoted string, resulting in "HOM"
   - Then "E" is added as a literal character
   - Then `$USER` is expanded normally

## 2. Handling Localization Strings ($"...")

When Bash encounters `$"string"`, it processes the string for localization. In a simple shell, this might just be treated as a regular string.

### Implementation Approach:

1. In the `expand_variables` function, detect this pattern:
   ```c
   if (input[i] == '$' && input[i+1] == '"') {
       // This is a localization string, not a variable
       process_localization_string(input, &i, &result);
   }
   ```

2. Create a function `process_localization_string` that:
   - Extracts the content between the quotes
   - Adds it to the result with any necessary localization processing
   - Updates the position index

## 3. Variable Name Parsing Fix

For proper variable name parsing, ensure:

1. Update `skip_variable_name` to strictly follow POSIX rules:
   ```c
   int skip_variable_name(const char *s) {
       int i = 0;
       
       // First character must be alpha or underscore
       if (!s || !*s || !(ft_isalpha(s[0]) || s[0] == '_'))
           return 0;
           
       i = 1;
       // Subsequent characters must be alphanumeric or underscore
       while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
           i++;
           
       return i;
   }
   ```

2. Ensure `handle_dollar` properly uses this function to extract only valid variable names.

## 4. Testing Strategy

Create test cases that specifically test:

1. ANSI-C quoting: `$'\n'` should be a newline, not a variable expansion
2. Localization strings: `$"hello"` 
3. Variable followed by text: `$HOMEdir` should not find a variable named "HOMEdir"
4. Mixed cases: `$'HOM'E$USER` should be "HOME" + value of USER variable

Compare all results with how Bash handles these cases.
