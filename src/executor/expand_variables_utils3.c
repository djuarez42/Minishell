/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils3.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 01:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/31 01:30:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdbool.h>

/*
 * Check if a string contains a sequence that looks like mixed quotes
 * Examples: "1"$X"2", '1'$X'2', "1"$X'2', '1'$X"2"
 * These sequences require special handling for variable expansion
 */
bool is_mixed_quotes(const char *str)
{
    if (!str)
        return (false);
    
    bool has_double = false;
    bool has_single = false;
    bool has_dollar = false;
    int i = 0;
    
    while (str[i])
    {
        // Check for quotes
        if (str[i] == '\"')
            has_double = true;
        else if (str[i] == '\'')
            has_single = true;
        // Check for dollar sign
        else if (str[i] == '$')
            has_dollar = true;
        
        i++;
    }
    
    // If we have dollar sign and at least one type of quote, 
    // or both types of quotes, consider it mixed quotes
    return ((has_dollar && (has_double || has_single)) || 
            (has_double && has_single));
}

/*
 * Count occurrences of quote characters (both single and double)
 * Used to determine if a string has balanced quotes
 */
void count_quotes(const char *str, int *single_count, int *double_count)
{
    int i = 0;
    *single_count = 0;
    *double_count = 0;
    
    while (str && str[i])
    {
        if (str[i] == '\'')
            (*single_count)++;
        else if (str[i] == '\"')
            (*double_count)++;
        i++;
    }
}

/*
 * Check if a string has a mixed quote pattern that could cause issues
 * with variable expansion, specifically for "$X" followed or preceded
 * by other quoted text
 */
bool has_problematic_quotes(const char *str)
{
    if (!str)
        return (false);
    
    int i = 0;
    bool in_quotes = false;
    char quote_char = '\0';
    
    while (str[i])
    {
        // Handle quote start/end
        if ((str[i] == '\'' || str[i] == '\"') && 
            (!in_quotes || quote_char == str[i]))
        {
            if (!in_quotes)
            {
                in_quotes = true;
                quote_char = str[i];
            }
            else
            {
                in_quotes = false;
                quote_char = '\0';
            }
        }
        
        // Look for $ outside of single quotes or immediately after a quote ends
        if (str[i] == '$' && (!in_quotes || quote_char != '\''))
        {
            // This is a pattern we need to handle specially
            return (true);
        }
        
        i++;
    }
    
    return (false);
}
