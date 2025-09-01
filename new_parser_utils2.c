/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_parser_utils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 21:21:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 21:35:31 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

t_redir *create_redir(t_token *cur)
{
	t_redir *redir;
	t_fragment *frag;

	if (!cur || !cur->next || !cur->next->fragments)
		return (NULL);

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);

	redir->type = cur->type;
	
	frag = cur->next->fragments;
	redir->quoted = frag->quote_type != QUOTE_NONE;
	redir->file = ft_strdup(frag->text);
	if (!redir->file)
		return (free(redir), NULL);

	redir->next = NULL;
	return (redir);
}

bool	is_quoted(const char *str)
{
	int	len;

	len = ft_strlen(str);
	if (len >= 2)
	{
		if ((str[0] == '"' && str[len - 1] == '"')
			|| (str[0] == '\'' && str[len - 1] == '\''))
			return (true);
	}
	return (false);
}

void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*last)->next = new_cmd;
	*last = new_cmd;
}

int	add_argument(t_cmd *cmd, char *value, t_quote_type quote, int *argc)
{
	cmd->argv[*argc] = ft_strdup(value);
	if (!cmd->argv[*argc])
	{
		while (--(*argc) >= 0)
			free(cmd->argv[*argc]);
		free(cmd->argv);
		cmd->argv = NULL;
		return (0);
	}
	cmd->argv_quote[*argc] = quote;
	(*argc)++;
	return (1);
}

t_cmd	*create_cmd_node(t_token **cur)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->argv_quote = NULL;
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
