/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_quotes_fix.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/08 21:31:38 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Fix for the issue with $? inside double quotes
 * 
 * The problem occurs with commands like `echo "$?"` where $? inside double quotes
 * should expand to the exit status of the previous command.
 * 
 * The issue is in how the shell handles $ followed by special characters inside quotes.
 * Below is the corrected version of handle_dollar function to properly handle special
 * characters like ? inside double quotes.
 */

char *handle_dollar_quotes_fix(const char *input, int *i, char **envp, t_exec_state *state)
{
    int     start;
    int     len;
    char    *res;
    char    *name;

    if (!input || !i)
        return (NULL);
    
    start = *i + 1;
    
    // Handle $"string" syntax - should be treated as literal string
    if (input[start] == '"')
    {
        *i = start;  // Skip just the $ but keep the quote
        return (ft_strdup(""));  // Return empty string, the quote will be processed normally
    }
    // Handle $'string' syntax - ANSI-C quoting
    else if (input[start] == '\'')
    {
        // Preserve the entire $'...' sequence for later processing in bi_echo
        // We'll return it as is and handle it during echo builtin execution
        int end = start + 1;
        while (input[end] && input[end] != '\'')
            end++;
            
        if (input[end] == '\'') // Found closing quote
        {
            // Create a string that includes $' and the closing '
            char *ansi_c_str = ft_substr(input, *i, end - *i + 1);
            *i = end + 1; // Skip past the closing quote
            
            return ansi_c_str;
        }
        else // No closing quote found
        {
            *i = start;
            return (ft_strdup("$"));
        }
    }
    // Special character handling - important for $? inside quotes
    else if (input[start] == '?')
    {
        *i = start + 1;  // Skip past ?
        return (expand_exit_status(state));
    }
    else if (input[start] == '$')
    {
        *i = start + 1;  // Skip past $
        return (ft_strdup("$"));
    }
    
    // End of string check
    if (input[start] == '\0')
    {
        *i = *i + 1;
        return (ft_strdup("$"));
    }
    
    // Variable name handling
    if (!is_var_start(input[start]))
    {
        *i = *i + 1;
        return (ft_strdup("$"));
    }
    
    len = skip_variable_name(input + start);
    if (len == 0)
    {
        *i = *i + 1;
        return (ft_strdup("$"));
    }
    
    name = ft_substr(input, start, len);
    if (!name)
        return (NULL);
    
    res = expand_env_var(name, envp);
    free(name);
    *i = start + len;
    
    // Make sure we always return a valid string, even if the variable isn't found
    if (!res)
        return (ft_strdup(""));
        
    return (res);
}

/*
 * To use this fixed function:
 * 1. Include this file in your build
 * 2. Replace the call to handle_dollar in expand_variables.c with handle_dollar_quotes_fix
 * 3. Add the function prototype to executor.h
 */
