/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_string_fix_v2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 13:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/01 13:30:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This function checks if we're at a $"string" pattern
 */
int is_dollar_string_pattern(const char *input, int pos)
{
    if (!input || !input[pos] || !input[pos + 1])
        return (0);
    
    return (input[pos] == '$' && input[pos + 1] == '"');
}

/*
 * This function handles the special $"string" syntax by returning
 * the literal string without expanding variables inside
 */
char *process_dollar_string(const char *input, int *pos)
{
    int i = *pos;
    char *result;
    int start, end;
    
    // Skip the $ character
    i++;
    
    // Skip the opening quote
    i++;
    
    // Find the end of the quoted string
    start = i;
    end = start;
    while (input[end] && input[end] != '"')
        end++;
    
    // Extract the content of the quotes (without expanding variables)
    result = ft_substr(input, start, end - start);
    if (!result)
        return (NULL);
    
    // Skip past the closing quote if present
    if (input[end] == '"')
        *pos = end + 1;
    else
        *pos = end;
    
    return (result);
}
