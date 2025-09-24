/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils13.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 20:47:45 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 21:22:58 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*last)->next = new_cmd;
	*last = new_cmd;
}

static void	free_argv_partial(t_cmd *cmd, int argc)
{
	int		i;
	char	**argv;

	if (!cmd || !cmd->argv)
		return ;
	argv = cmd->argv;
	i = 0;
	if (argc >= 0)
	{
		while (i < argc && argv[i])
			free(argv[i++]);
	}
	else
	{
		while (argv[i])
			free(argv[i++]);
	}
	free(argv);
	cmd->argv = NULL;
}

void	free_partial_cmd(t_cmd *cmd, int argc)
{
	if (!cmd)
		return ;
	free_argv_partial(cmd, argc);
	if (cmd->argv_final_text)
	{
		free_str_array(cmd->argv_final_text);
		cmd->argv_final_text = NULL;
	}
	if (cmd->argv_first_word)
	{
		free(cmd->argv_first_word);
		cmd->argv_first_word = NULL;
	}
	if (cmd->argv_quote)
	{
		free(cmd->argv_quote);
		cmd->argv_quote = NULL;
	}
}
