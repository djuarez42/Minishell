/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils6.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:12:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 17:41:51 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*concat_fragments(t_fragment *frag)
{
	size_t	len;
	char	*res;

	if (!frag)
		return (NULL);
	len = calc_total_length(frag);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	copy_fragments_to_buffer(frag, res);
	return (res);
}

t_token	*append_token_eof(t_token *head)
{
	t_token	*new;
	t_token	*cur;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->fragments = NULL;
	new->final_text = NULL;
	new->type = TOKEN_EOF;
	new->has_space_before = false;
	new->next = NULL;
	if (!head)
		return (new);
	cur = head;
	while (cur->next)
		cur = cur->next;
	cur->next = new;
	return (head);
}

void	free_fragments(t_fragment *frag)
{
	t_fragment	*tmp;

	while (frag)
	{
		tmp = frag->next;
		if (frag->text)
			free(frag->text);
		if (frag->expanded_text)
			free(frag->expanded_text);
		free(frag);
		frag = tmp;
	}
}

void	free_token_list(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		if (tokens->fragments)
			free_fragments(tokens->fragments);
		if (tokens->final_text)
			free(tokens->final_text);
		free(tokens);
		tokens = tmp;
	}
}

t_token_type	determine_token_type(char *str, t_quote_type quote)
{
	if (!str)
		return (TOKEN_WORD);
	if (quote != QUOTE_NONE)
		return (TOKEN_WORD);
	if (str[0] == '<' && str[1] == '<' && str[2] == '\0')
	{
		return (TOKEN_HEREDOC);
	}
	else if (str[0] == '>' && str[1] == '>' && str[2] == '\0')
	{
		return (TOKEN_APPEND);
	}
	else if (str[0] == '<' && str[1] == '\0')
	{
		return (TOKEN_REDIRECT_IN);
	}
	else if (str[0] == '>' && str[1] == '\0')
	{
		return (TOKEN_REDIRECT_OUT);
	}
	else if (str[0] == '|' && str[1] == '\0')
	{
		return (TOKEN_PIPE);
	}
	else
		return (TOKEN_WORD);
}
