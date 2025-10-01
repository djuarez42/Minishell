/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils6.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:12:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/01 17:31:05 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
