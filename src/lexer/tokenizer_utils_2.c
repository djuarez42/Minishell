/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 18:46:30 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/28 20:41:52 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

t_token_type	determine_token_type(char *str)
{
	if (str && str[0] == '<' && str[1] == '<' && str[2] == '\0')
		return (TOKEN_HEREDOC);
	else if (str && str[0] == '>' && str[1] == '>' && str[2] == '\0')
		return (TOKEN_APPEND);
	else if (str && str[0] == '<' && str[1] == '\0')
		return (TOKEN_REDIRECT_IN);
	else if (str && str[0] == '>' && str[1] == '\0')
		return (TOKEN_REDIRECT_OUT);
	else if (str && str[0] == '|' && str[1] == '\0')
		return (TOKEN_PIPE);
	else
		return (TOKEN_WORD);
}

t_token	*build_token_list(char **tokens, t_quote_type *quotes)
{
	t_token	*head;
	t_token	*cur;
	t_token	*new;
	int		i;

	head = NULL;
	cur = NULL;
	i = 0;
	while (tokens[i])
	{
		// Skip empty tokens
		if (tokens[i][0] == '\0')
		{
			i++;
			continue;
		}
		new = malloc(sizeof(t_token));
		if (!new)
			return (NULL);
		new->value = ft_strdup(tokens[i]);
		new->type = determine_token_type(tokens[i]);
		new->quote_type = quotes[i];
		new->next = NULL;
		if (!head)
			head = new;
		else
			cur->next = new;
		cur = new;
		i++;
	}
	return (append_token_eof(head, cur));
}

void	print_token_list(t_token *token)
{
	while (token)
	{
		printf("Type: %-15s Quote: %-7s Value: \"%s\"\n",
			token_type_str(token->type),
			quote_type_str(token->quote_type),
			token->value);
		token = token->next;
	}
}

void	free_token_list(t_token *head)
{
	t_token	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->value);
		free(tmp);
	}
}

t_token	*append_token_eof(t_token *head, t_token *cur)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->value = NULL;
	new->type = TOKEN_EOF;
	new->quote_type = QUOTE_NONE;
	new->next = NULL;
	if (!head)
		return (new);
	cur->next = (new);
	return (head);
}

void	classify_redirection_files(t_token *tokens)
{
	t_token	*cur;

	cur = tokens;
	while (cur)
	{
		if (cur->next)
		{
			if ((cur->type == TOKEN_REDIRECT_OUT || cur->type == TOKEN_APPEND) 
				&& cur->next->type == TOKEN_WORD)
			{
				cur->next->type = TOKEN_OUT_FILE;
			}
			else if ((cur->type == TOKEN_REDIRECT_IN || cur->type == TOKEN_HEREDOC) 
				&& cur->next->type == TOKEN_WORD)
			{
				cur->next->type = TOKEN_IN_FILE;
			}
		}
		cur = cur->next;
	}
}
