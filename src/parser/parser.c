/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/28 21:14:40 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

/* forward declare to avoid header ordering issues */
void	free_partial_cmd(t_cmd *cmd, int argc);

t_cmd	*parser_tokens(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*last;
	t_token	*cur;
	t_cmd	*new_cmd;

	head = NULL;
	last = NULL;
	cur = tokens;
	while (cur && cur->type != TOKEN_EOF)
	{
		/* Syntax: leading pipe is invalid */
		if (cur->type == TOKEN_PIPE)
			return (free_cmds(head), NULL);
		new_cmd = create_cmd_node(&cur);
		if (!new_cmd)
			return (free_cmds(head), NULL);
		add_cmd_node(&head, &last, new_cmd);
		/* After a command, a single pipe is allowed; reject double/trailing */
		if (cur && cur->type == TOKEN_PIPE)
		{
			if (!cur->next || cur->next->type == TOKEN_PIPE
				|| cur->next->type == TOKEN_EOF)
				return (free_cmds(head), NULL);
			cur = cur->next;
		}
	}
	return (head);
}

t_token	*parse_cmd_block(t_token *cur, t_cmd *cmd)
{
	cur = parse_arguments(cur, cmd);
	if (!cur)
		return (NULL);
	while (cur && (cur->type == TOKEN_REDIRECT_OUT
			|| cur->type == TOKEN_REDIRECT_IN
			|| cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC))
	{
		cur = parse_redirections(cur, cmd);
		if (!cur)
		{
/* redir parse failed: free argv/argv_quote and any redirs created */
			free_partial_cmd(cmd, -1);
			free_redirs(cmd->redirs);
			cmd->redirs = NULL;
			return (NULL);
		}
	}
	return (cur);
}

t_token	*parse_redirections(t_token *cur, t_cmd *cmd)
{
	t_redir	*new_redir;
	t_redir	*last;

	if (!cur || !cur->next)
		return (NULL);
	// Accept both WORD tokens and the new file types for compatibility
	if (cur->next->type != TOKEN_WORD 
		&& cur->next->type != TOKEN_IN_FILE 
		&& cur->next->type != TOKEN_OUT_FILE)
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

t_token	*parse_arguments(t_token *cur, t_cmd *cmd)
{
	int	argc;

	argc = 0;
	if (!init_cmd_args(cmd))
		return (NULL);
	while (cur && cur->type != TOKEN_PIPE && cur->type != TOKEN_EOF)
	{
		if (cur->type == TOKEN_WORD)
		{
			if (!process_token(cmd, cur, &argc))
			{
				free_partial_cmd(cmd, argc);
				return (NULL);
			}
		}
		else if (cur->type == TOKEN_REDIRECT_OUT
			|| cur->type == TOKEN_REDIRECT_IN
			|| cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC)
			break ;
		cur = cur->next;
	}
	cmd->argv[argc] = NULL;
	cmd->argv_quote[argc] = QUOTE_NONE;
	return (cur);
}
