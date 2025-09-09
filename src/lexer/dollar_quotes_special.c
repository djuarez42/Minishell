/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_quotes_special.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:45:22 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/09 21:50:03 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"
#include <stdio.h>

/*
 * This file implements special handling for:
 * 1. $"string" syntax in bash which is used for locale-specific translations.
 * 2. $'string' syntax in bash which is used for ANSI-C quoting.
 * In our minishell implementation, we handle both syntax forms correctly.
 */

/*
 * Check if the current position represents a $"string" or $'string' construct
 * Returns 1 if it is $"string" or $'string', 0 otherwise
 */
int is_dollar_quotes_construct(const char *input, int pos)
{
    if (!input || input[pos] != '$')
        return 0;
        
    // Check if the $ is followed by " or '
    if (input[pos + 1] != '"' && input[pos + 1] != '\'')
        return 0;
        
    return 1;
}

/*
 * Process a $"string" or $'string' construct by returning just the string without expanding variables
 * Updates the position to after the closing quote
 * Returns the content of the string (not including the quotes)
 */
char *process_dollar_quotes(const char *input, int *pos)
{
    int start;
    int end;
    char *result;
    char quote_type;
    
    quote_type = input[*pos + 1];  // Get the quote type (" or ')
    start = *pos + 2;              // Skip past $" or $'
    end = start;
    
    // Find the closing quote (matching the opening one)
    while (input[end] && input[end] != quote_type)
        end++;
        
    // Extract the string between quotes (without expansion)
    result = ft_substr(input, start, end - start);
    
    // Update position to after the closing quote if found
    if (input[end] == quote_type)
        *pos = end + 1;
    else
        *pos = end;
        
    return result;
}

/*
 * Integration function to be called from the lexer
 * Returns 1 if a $"string" was processed, 0 otherwise
 */
int handle_dollar_quotes_in_lexer(const char *input, int *pos, char **result)
{
    char *quoted_content;
    
    if (!is_dollar_quotes_construct(input, *pos))
        return 0;
    
    quoted_content = process_dollar_quotes(input, pos);
    if (!quoted_content)
        return 0;
    
    // Append the content to the result without expansion
    *result = str_append(*result, quoted_content);
    free(quoted_content);
    
    return 1;
}
