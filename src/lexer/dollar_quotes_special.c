/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_quotes_special.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:45:22 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/30 21:45:22 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"
#include <stdio.h>

/*
 * This file implements special handling for the $"string" syntax
 * in bash which is used for locale-specific translations.
 * In our minishell implementation, we don't actually do translations
 * but we need to handle the syntax correctly to match bash's behavior.
 */

/*
 * Check if the current position represents a $"string" construct
 * Returns 1 if it is $"string", 0 otherwise
 */
int is_dollar_quotes_construct(const char *input, int pos)
{
    if (!input || input[pos] != '$')
        return 0;
        
    // Check if the $ is followed by "
    if (input[pos + 1] != '"')
        return 0;
        
    return 1;
}

/*
 * Process a $"string" construct by returning just the string without expanding variables
 * Updates the position to after the closing quote
 * Returns the content of the string (not including the quotes)
 */
char *process_dollar_quotes(const char *input, int *pos)
{
    int start = *pos + 2;  // Skip past $"
    int end = start;
    char *result;
    
    // Find the closing double quote
    while (input[end] && input[end] != '"')
        end++;
        
    // Extract the string between quotes (without expansion)
    result = ft_substr(input, start, end - start);
    
    // Update position to after the closing quote if found
    if (input[end] == '"')
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
