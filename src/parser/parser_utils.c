/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/07/24 17:23:52 by djuarez          ###   ########.fr       */
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
			{
				free(cmd->argv[i]);
				i++;
			}
			free(cmd->argv);
		}
		free_redirs(cmd->redirs);
		free(cmd);
		cmd = tmp;
	}
}

t_token	*parse_arguments(t_token *cur, t_cmd *cmd)
{
	int	argc;

	argc = 0;
	cmd->argv = malloc (sizeof(char *) * MAX_ARGS);
	if (!cmd->argv)
		return (NULL);
	while (cur && cur->type != TOKEN_PIPE && cur->type != TOKEN_EOF)
	{
		if (cur->type == TOKEN_WORD)
		{
			if (argc >= MAX_ARGS - 1)
				break ;
			if (!add_argument(cmd, cur->value, &argc))
				return (NULL);
		}
		else if (cur->type == TOKEN_REDIRECT_OUT
			|| cur->type == TOKEN_REDIRECT_IN
			|| cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC)
			break ;
		cur = cur->next;
	}
	cmd->argv[argc] = NULL;
	return (cur);
}

t_token	*parse_cmd_block(t_token *cur, t_cmd *cmd)
{
	cur = parse_arguments(cur, cmd);
	while (cur && (cur->type == TOKEN_REDIRECT_OUT
			|| cur->type == TOKEN_REDIRECT_IN
			|| cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC))
		cur = parse_redirections(cur, cmd);
	return (cur);
}

t_token	*parse_redirections(t_token *cur, t_cmd *cmd)
{
	t_redir	*new_redir;
	t_redir	*last;

	if (!cur || !cur->next || cur->next->type != TOKEN_WORD)
		return (NULL);
	new_redir = create_redir(cur);
	if (!new_redir)
		return (NULL);
	if (!cmd->redirs)
		cmd->redirs = new_redir;
	else
	{
		last = cmd->redirs;
		while (last->next)
			last = last->next;
		last->next = new_redir;
	}
	return (cur->next->next);
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
