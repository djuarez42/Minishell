#include "minishell.h"

/*
 * This function checks if we're at a $"string" pattern and
 * processes it correctly according to bash behavior.
 */
int is_dollar_quotes_pattern(const char *input, int pos)
{
    if (!input || !input[pos])
        return 0;
        
    return (input[pos] == '$' && input[pos + 1] == '"');
}

/*
 * Process a $"string" pattern, returning the literal string
 * without variable expansion.
 */
char *process_dollar_quotes(const char *input, int *pos)
{
    char *result;
    int start, end;
    
    if (!input || !is_dollar_quotes_pattern(input, *pos))
        return NULL;
    
    // Skip the $ character
    (*pos)++;
    
    // Skip the opening quote
    (*pos)++;
    
    // Find the end of the quoted string
    start = *pos;
    end = start;
    while (input[end] && input[end] != '"')
        end++;
        
    // Extract the content of the quotes
    result = ft_substr(input, start, end - start);
    
    // Skip past the closing quote if present
    if (input[end] == '"')
        *pos = end + 1;
    else
        *pos = end;
        
    return result;
}
