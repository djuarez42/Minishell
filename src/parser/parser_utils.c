/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 11:57:13 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/* Free argv entries up to argc (if argc >= 0), and argv/argv_quote arrays.
   Does not free cmd itself. */
void	free_partial_cmd(t_cmd *cmd, int argc)
{
	int i;

	if (!cmd)
		return;
	if (cmd->argv)
	{
		if (argc >= 0)
		{
			i = 0;
			while (i < argc)
			{
				free(cmd->argv[i]);
				i++;
			}
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	if (cmd->argv_quote)
	{
		free(cmd->argv_quote);
		cmd->argv_quote = NULL;
	}
}

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

int process_token(t_cmd *cmd, t_token *cur, int *argc)
{
	t_fragment *frag;
	char *arg;
	int len = 0, pos = 0;

	if (*argc >= MAX_ARGS - 1)
		return (0);
		
	// Calculate total size needed for all fragments
	frag = cur->fragments;
	while (frag)
	{
		len += ft_strlen(frag->text);
		frag = frag->next;
	}
	
	// Allocate and combine fragments
	arg = (char*)malloc(len + 1);
	if (!arg)
		return (0);
	
	arg[0] = '\0';
	pos = 0;
	
	frag = cur->fragments;
	while (frag)
	{
		ft_memcpy(arg + pos, frag->text, ft_strlen(frag->text));
		pos += ft_strlen(frag->text);
		frag = frag->next;
	}
	arg[pos] = '\0';

	cmd->argv[*argc] = arg;
	cmd->argv_quote[*argc] = cur->fragments ? cur->fragments->quote_type : QUOTE_NONE;
	(*argc)++;
	return (1);
}



