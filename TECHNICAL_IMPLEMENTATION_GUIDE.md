# Technical Implementation Guide for Variable Expansion Fixes

## Overview

This guide provides detailed code changes needed to fix the variable expansion issues in the Minishell project, with a focus on handling special cases like `$'HOM'E$USER` correctly.

## 1. Modify `expand_variables.c`

The main expansion function needs to be updated to handle ANSI-C quoted strings and localization strings:

```c
char *expand_variables(const char *input, char **envp, t_exec_state *state)
{
    int     i;
    char    *tmp;
    char    *piece;

    if (!input)
        return (NULL);
    i = 0;
    tmp = NULL;
    piece = NULL;
    while (input[i])
    {
        // Check for backslash escaping a $
        if (input[i] == '\\' && input[i+1] == '$')
        {
            // Add just the literal $ to output and skip both characters
            piece = ft_strdup("$");
            if (!piece)
            {
                free(tmp);
                return (NULL);
            }
            tmp = str_append(tmp, piece);
            free(piece);
            i += 2; // Skip both backslash and dollar
        }
        // Check for ANSI-C quoted string ($'...')
        else if (input[i] == '$' && input[i+1] == '\'')
        {
            piece = process_ansi_c_quote(input, &i, envp, state);
            if (!piece)
            {
                free(tmp);
                return (NULL);
            }
            tmp = str_append(tmp, piece);
            free(piece);
        }
        // Check for localization string ($"...")
        else if (input[i] == '$' && input[i+1] == '"')
        {
            piece = process_localization_string(input, &i, envp, state);
            if (!piece)
            {
                free(tmp);
                return (NULL);
            }
            tmp = str_append(tmp, piece);
            free(piece);
        }
        else if (input[i] == '$')
        {
            piece = handle_dollar(input, &i, envp, state);
            if (!piece)
            {
                free(tmp);
                return (NULL);
            }
            tmp = str_append(tmp, piece);
            if (!tmp)
            {
                free(piece);
                return (NULL);
            }
            free(piece);
        }
        else
        {
            tmp = extract_plain_text(input, &i, tmp);
            if (!tmp)
                return (NULL);
        }
    }
    return (tmp);
}
```

## 2. Add New Functions to `expand_variables_utils.c`

Add these new functions to handle ANSI-C quoted strings and localization strings:

```c
/* Process an ANSI-C quoted string like $'...' */
char *process_ansi_c_quote(const char *input, int *i, char **envp, t_exec_state *state)
{
    int     start;
    int     end;
    char    *content;
    char    *result;

    // Skip the $ and opening quote
    *i += 2;
    
    // Find the closing quote
    start = *i;
    while (input[*i] && input[*i] != '\'')
        (*i)++;
    
    if (input[*i] == '\'')
    {
        end = *i;
        // Extract content between quotes
        content = ft_substr(input, start, end - start);
        if (!content)
            return (NULL);
        
        // In a simple implementation, just return the content
        // For full ANSI-C compatibility, you'd process escape sequences here
        result = ft_strdup(content);
        free(content);
        
        // Skip the closing quote
        (*i)++;
        return (result);
    }
    else
    {
        // No closing quote found, return as literal
        return (ft_strdup("$'"));
    }
}

/* Process a localization string like $"..." */
char *process_localization_string(const char *input, int *i, char **envp, t_exec_state *state)
{
    int     start;
    int     end;
    char    *content;
    char    *result;

    // Skip the $ and opening quote
    *i += 2;
    
    // Find the closing quote
    start = *i;
    while (input[*i] && input[*i] != '"')
        (*i)++;
    
    if (input[*i] == '"')
    {
        end = *i;
        // Extract content between quotes
        content = ft_substr(input, start, end - start);
        if (!content)
            return (NULL);
        
        // In a simple implementation, just return the content
        // For full localization support, you'd process it differently
        result = ft_strdup(content);
        free(content);
        
        // Skip the closing quote
        (*i)++;
        return (result);
    }
    else
    {
        // No closing quote found, return as literal
        return (ft_strdup("$\""));
    }
}
```

## 3. Revise Function Prototypes in Headers

Add the new function prototypes to `minishell.h` or the appropriate header file:

```c
/* New function prototypes */
char *process_ansi_c_quote(const char *input, int *i, char **envp, t_exec_state *state);
char *process_localization_string(const char *input, int *i, char **envp, t_exec_state *state);
```

## 4. Testing the Implementation

After implementing these changes:

1. Compile the project
2. Test with the specific case that was failing: `$'HOM'E$USER`
3. Run through the test plan to ensure all variable expansion cases work correctly

## 5. Advanced Features (Optional)

For a more complete implementation:

1. **ANSI-C Escape Processing**: Enhance `process_ansi_c_quote` to handle escape sequences:
   - `\a` (alert/bell)
   - `\b` (backspace)
   - `\e` or `\E` (escape)
   - `\f` (form feed)
   - `\n` (newline)
   - `\r` (carriage return)
   - `\t` (horizontal tab)
   - `\v` (vertical tab)
   - `\\` (backslash)
   - `\'` (single quote)
   - `\"` (double quote)
   - `\nnn` (octal value)
   - `\xHH` (hex value)
   - `\uHHHH` (Unicode character)
   - `\UHHHHHHHH` (Unicode character)

2. **Localization Support**: If needed, enhance `process_localization_string` to handle actual localization.

## 6. Error Handling

Ensure robust error handling throughout:

1. Memory allocation failures
2. Malformed input
3. Edge cases (empty strings, unterminated quotes, etc.)

## 7. Integration Notes

When integrating these changes:

1. Be careful not to break existing functionality
2. Consider the impact on parsing and execution
3. Test thoroughly with the existing test suite
4. Add new test cases for the special handling of ANSI-C quoted strings and localization strings
