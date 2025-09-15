/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 21:10:57 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 00:51:32 by djuarez          ###   ########.fr       */
/*   Created: 2025/08/31 03:34:47 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/08/31 03:34:47 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*create_operator_token(t_fragment **cur, bool *space_before)
{
	t_token		*tok;
	t_fragment	*frag_copy;

	tok = create_token(determine_token_type((*cur)->text,
				(*cur)->quote_type), *space_before);
	*space_before = (*cur)->has_space_after;
	frag_copy = new_fragment((*cur)->text, strlen((*cur)->text),
			(*cur)->quote_type, (*cur)->has_space_after);
	append_fragment(&tok->fragments, frag_copy);
	tok->final_text = concat_fragments(tok->fragments);
	*cur = (*cur)->next;
	return (tok);
}

t_token	*create_word_token(t_fragment **cur, bool *space_before)
{
	t_token		*tok;
	t_fragment	*frag_copy;

	tok = create_token(TOKEN_WORD, *space_before);
	*space_before = false;
	while (*cur)
	{
		frag_copy = new_fragment((*cur)->text, strlen((*cur)->text),
				(*cur)->quote_type, (*cur)->has_space_after);
		append_fragment(&tok->fragments, frag_copy);
		if ((*cur)->has_space_after)
		{
			*space_before = true;
			*cur = (*cur)->next;
			break ;
		}
		*cur = (*cur)->next;
		if (*cur && determine_token_type((*cur)->text,
				(*cur)->quote_type) != TOKEN_WORD)
			break ;
	}
	tok->type = determine_token_type(tok->fragments->text,
			tok->fragments->quote_type);
	tok->final_text = concat_fragments(tok->fragments);
	return (tok);
}

void	skip_empty_fragments(t_fragment **cur)
{
	while (*cur && strlen((*cur)->text) == 0
		&& (*cur)->quote_type == QUOTE_NONE)
	{
		*cur = (*cur)->next;
	}
}

t_token	*tokenize_input(const char *input)
{
	t_fragment	*frags;
	t_fragment	*cur;
	t_token		*tokens;
	t_token		*last_token;
	t_token		*tok;
	bool		space_before;

	if (check_unmatched_quotes(input))
		return (NULL);
	frags = parse_mixed_fragments(input);
	if (!frags)
		return (NULL);
	tokens = NULL;
	last_token = NULL;
	space_before = false;
	cur = frags;
	while (cur)
	{
		skip_empty_fragments(&cur);
		if (!cur)
			break ;
		if (determine_token_type(cur->text, cur->quote_type) != TOKEN_WORD)
			tok = create_operator_token(&cur, &space_before);
		else
			tok = create_word_token(&cur, &space_before);
		if (!tokens)
			tokens = tok;
		else
			last_token->next = tok;
		last_token = tok;
	}
	free_fragments(frags);
	tokens = append_token_eof(tokens);
	return (tokens);
}
