/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_quotes_fix.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/30 18:43:18 by ekakhmad         ###   ########.fr       */
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
    
    // Special character handling - important for $? inside quotes
    if (input[start] == '?')
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
    return (res);
}

/*
 * To use this fixed function:
 * 1. Include this file in your build
 * 2. Replace the call to handle_dollar in expand_variables.c with handle_dollar_quotes_fix
 * 3. Add the function prototype to executor.h
 */
