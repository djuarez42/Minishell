/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd_inplace.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 14:45:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/01 14:45:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Expands variables in a command structure in-place
 * Returns 0 on success, -1 on failure
 */
int expand_cmd_inplace(t_cmd *cmd, char **envp, t_exec_state *state)
{
    int i;
    char *expanded;
    
    if (!cmd)
        return (-1);
    
    // Expand arguments (including command name which is argv[0])
    if (cmd->argv)
    {
        i = 0;
        while (cmd->argv[i])
        {
            expanded = expand_variables(cmd->argv[i], envp, state, 
                (cmd->argv_quote && cmd->argv_quote[i]) ? cmd->argv_quote[i] : QUOTE_NONE);
            if (!expanded)
                return (-1);
            free(cmd->argv[i]);
            cmd->argv[i] = expanded;
            i++;
        }
    }
    
    // Expand redirections
    if (cmd->redirs)
    {
        t_redir *redir = cmd->redirs;
        while (redir)
        {
            if (redir->file)
            {
                expanded = expand_variables(redir->file, envp, state, 
                    redir->quoted ? QUOTE_DOUBLE : QUOTE_NONE);
                if (!expanded)
                    return (-1);
                free(redir->file);
                redir->file = expanded;
            }
            redir = redir->next;
        }
    }
    
    return (0);
}
