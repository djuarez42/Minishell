/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd_inplace.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 14:45:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/02 21:33:24 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Expands variables in a command structure in-place
 * Returns 0 on success, -1 on failure
 */
int	expand_cmd_inplace(t_cmd *cmd, char **envp, t_exec_state *state)
{
	int		i;
	char	*expanded;
	t_redir	*redir;
	int		quote_type;

	if (!cmd)
		return (-1);
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		{
			quote_type = (cmd->argv_quote && cmd->argv_quote[i])
				? cmd->argv_quote[i] : QUOTE_NONE;
			expanded = expand_variables(cmd->argv[i], envp, state, quote_type);
			if (!expanded)
				return (-1);
			free(cmd->argv[i]);
			cmd->argv[i] = expanded;
			i++;
		}
	}
	if (cmd->redirs)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->file)
			{
				quote_type = redir->quoted ? QUOTE_DOUBLE : QUOTE_NONE;
				expanded = expand_variables(redir->file, envp, state, quote_type);
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