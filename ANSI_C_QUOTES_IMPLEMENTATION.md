# Special Case Implementation: `$'HOM'E$USER`

## Understanding the Problem

The expression `$'HOM'E$USER` is a complex case that combines several features:

1. `$'HOM'` - This is an ANSI-C quoted string (not a variable)
2. `E` - A literal character
3. `$USER` - A variable expansion

In Bash, this would be processed as:
- `$'HOM'` → "HOM" (ANSI-C quoted string)
- `E` → "E" (literal character)
- `$USER` → Value of USER variable

## Current Implementation Issues

The current implementation likely:
1. Tries to find a variable named `'HOM'E` which doesn't exist
2. Then expands `$USER` correctly

## Solution Approach

### 1. Modify `expand_variables` to recognize ANSI-C quoting

```c
char *expand_variables(const char *input, char **envp, t_exec_state *state)
{
    int i = 0;
    char *result = NULL;
    
    while (input[i])
    {
        if (input[i] == '$')
        {
            // Check for ANSI-C quoting
            if (input[i+1] == '\'')
            {
                i++; // Skip the $
                char *ansi_c_str = process_ansi_c_quote(input, &i, envp, state);
                result = str_append(result, ansi_c_str);
                free(ansi_c_str);
            }
            // Check for localization string
            else if (input[i+1] == '"')
            {
                i++; // Skip the $
                char *loc_str = process_localization_string(input, &i, envp, state);
                result = str_append(result, loc_str);
                free(loc_str);
            }
            // Regular variable expansion
            else
            {
                char *var_expansion = handle_dollar(input, &i, envp, state);
                result = str_append(result, var_expansion);
                free(var_expansion);
            }
        }
        else
        {
            // Handle regular text
            char *text = extract_regular_text(input, &i);
            result = str_append(result, text);
            free(text);
        }
    }
    
    return result;
}
```

### 2. Implement ANSI-C quote processing

```c
char *process_ansi_c_quote(const char *input, int *pos, char **envp, t_exec_state *state)
{
    // Skip the opening quote
    (*pos)++;
    
    int start = *pos;
    char *result = NULL;
    
    // Find the closing quote
    while (input[*pos] && input[*pos] != '\'')
        (*pos)++;
    
    if (input[*pos] == '\'')
    {
        // Extract the content between quotes
        char *content = ft_substr(input, start, *pos - start);
        
        // Process any backslash escapes according to ANSI-C rules
        result = process_ansi_c_escapes(content);
        
        free(content);
        (*pos)++; // Skip the closing quote
    }
    else
    {
        // No closing quote found, treat as literal
        result = ft_strdup("$'");
        *pos = start; // Reset position
    }
    
    return result;
}
```

### 3. Implement a basic ANSI-C escape processor

```c
char *process_ansi_c_escapes(const char *str)
{
    // For a simple implementation, just return the string as-is
    // In a full implementation, you would process escapes like \n, \t, etc.
    return ft_strdup(str);
}
```

### 4. Test Case for `$'HOM'E$USER`

To test this specific case:

```c
// Given:
// - The input string is "$'HOM'E$USER"
// - USER="testuser"

// Expected output: "HOMEtestuser"

// Step-by-step processing:
// 1. Encounter '$' followed by '\'' → Process as ANSI-C quoted string
// 2. Extract "HOM" from between the quotes
// 3. Process any ANSI-C escapes in "HOM" (none in this case)
// 4. Append "HOM" to the result
// 5. Encounter 'E' → Append "E" to the result
// 6. Encounter '$USER' → Expand to "testuser" and append to the result
// 7. Final result: "HOMEtestuser"
```

## Implementation Phases

1. Add detection for ANSI-C quoting and localization strings
2. Implement the basic ANSI-C quote processing
3. Test with simple cases
4. Add full ANSI-C escape sequence processing if needed
5. Integrate with the rest of the variable expansion logic
