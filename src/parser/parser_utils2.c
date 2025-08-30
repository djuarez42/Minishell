/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 21:21:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 18:43:18 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

t_redir	*create_redir(t_token *cur)
{
	t_redir	*redir;
	int		has_quotes;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = cur->type;
	
	/* For heredoc, we need special handling of quoted delimiters */
	if (cur->type == TOKEN_HEREDOC)
	{
		/* First check the token's quote_type */
		redir->quoted = (cur->next->quote_type != QUOTE_NONE);
		
		/* Check for any quotes in the value (this catches mixed quoted cases) */
		has_quotes = 0;
		char *str = cur->next->value;
		int i = 0;
		while (str && str[i])
		{
			if (str[i] == '\'' || str[i] == '\"')
			{
				has_quotes = 1;
				redir->quoted = true;
				break;
			}
			i++;
		}
		
		/* If the delimiter has quotes, we need to create a clean version without quotes.
		   This is essential for correctly matching the delimiter when processing the heredoc. */
		if (has_quotes)
		{
			char *clean = malloc(ft_strlen(str) + 1);
			if (!clean)
				return (free(redir), NULL);
			
			/* Copy the string without any quotes */
			int j = 0;
			i = 0;
			while (str[i])
			{
				if (str[i] != '\'' && str[i] != '\"')
					clean[j++] = str[i];
				i++;
			}
			clean[j] = '\0';
			
			redir->file = clean;
		}
		else
		{
			redir->file = ft_strdup(cur->next->value);
			if (!redir->file)
				return (free(redir), NULL);
		}
	}
	else
	{
		/* For other redirection types, use the standard quote type check */
		redir->quoted = (cur->next->quote_type != QUOTE_NONE);
		redir->file = ft_strdup(cur->next->value);
		if (!redir->file)
			return (free(redir), NULL);
	}
	
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
