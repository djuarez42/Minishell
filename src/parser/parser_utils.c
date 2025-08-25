/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/23 20:46:54 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;
	int		i;

	while (cmd)
	{
		tmp = cmd->next;
		if (cmd->argv)
		{
			i = 0;
			while (cmd->argv[i])
				free(cmd->argv[i++]);
			free(cmd->argv);
		}
		if (cmd->argv_quote)
			free(cmd->argv_quote);
		free_redirs(cmd->redirs);
		free(cmd);
		cmd = tmp;
	}
}

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		free(redir);
		redir = tmp;
	}
}

int	init_cmd_args(t_cmd *cmd)
{
	cmd->argv = malloc(sizeof(char *) * MAX_ARGS);
	cmd->argv_quote = malloc(sizeof(t_quote_type) * MAX_ARGS);
	if (!cmd->argv || !cmd->argv_quote)
	{
		free(cmd->argv);
		free(cmd->argv_quote);
		return (0);
	}
	return (1);
}

int	process_token(t_cmd *cmd, t_token *cur, int *argc)
{
	char	*clean;

	if (*argc >= MAX_ARGS - 1)
		return (0);
	clean = remove_quotes(cur->value);
	if (!clean)
		return (0);
	if (!add_argument(cmd, clean, cur->quote_type, argc))
	{
		free(clean);
		return (0);
	}
	free(clean);
	return (1);
}
