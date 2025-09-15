/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils3.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 01:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/15 18:12:08 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdbool.h>

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
