/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/06/10 15:30:12 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*last)->next = new_cmd;
	*last = new_cmd;
}

t_cmd	*create_cmd_node(t_token **cur)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->pipe = 0;
	cmd->next = NULL;
	*cur = parse_cmd_block(*cur, cmd);
	if (!*cur)
		return (free(cmd), NULL);
	if ((*cur)->type == TOKEN_PIPE)
	{
		cmd->pipe = 1;
		*cur = (*cur)->next;
	}
	return (cmd);
}

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
		new_cmd = create_cmd_node(&cur);
		if (!new_cmd)
			return (free_cmds(head), NULL);
		add_cmd_node(&head, &last, new_cmd);
	}
	return (head);
}
