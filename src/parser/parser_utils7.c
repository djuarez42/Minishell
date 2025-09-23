/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils7.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 20:57:25 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 21:32:40 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_cmd_arrays(t_cmd *cmd)
{
	int	i;

	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
	}
	if (cmd->argv_final_text)
	{
		i = 0;
		while (cmd->argv_final_text[i])
			free(cmd->argv_final_text[i++]);
		free(cmd->argv_final_text);
	}
	if (cmd->argv_quote)
		free(cmd->argv_quote);
	if (cmd->argv_first_word)
		free(cmd->argv_first_word);
	if (cmd->redirs)
		free_redirs(cmd->redirs);
}

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free_cmd_arrays(cmd);
		free(cmd);
		cmd = tmp;
	}
}

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;
	int		i;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		if (redir->heredoc_content)
		{
			i = 0;
			while (redir->heredoc_content[i])
				free(redir->heredoc_content[i++]);
			free(redir->heredoc_content);
		}
		free(redir);
		redir = tmp;
	}
}

void	free_str_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}
