/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 19:20:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/28 21:14:40 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*tokenize_input(const char *input)
{
	char			**tokens;
	t_token			*token_list;
	t_quote_type	*quotes;

	if (!input)
		return (NULL);
	tokens = clean_input_quotes(input, &quotes);
	if (!tokens)
		return (NULL);
	token_list = build_token_list(tokens, quotes);
	free_tokens(tokens, -1);
	free(quotes);
	if (token_list)
		classify_redirection_files(token_list);
	return (token_list);
}

char	**reconstruct_words(const char *input, t_quote_type *last_quote,
			t_quote_type **quotes_out)
{
	t_reconstruct	r;

	if (!init_tokens_and_quotes(&r.tokens, quotes_out))
		return (NULL);
	r.i = 0;
	r.tok_i = 0;
	r.tmp = NULL;
	while (input[r.i])
	{
		r.last_i = r.i;
		r.i = process_spaces_and_quotes(input, r.i, &r.tmp, last_quote);
		if (r.i == -1)
			break ;
		/* If we are at an operator, flush tmp (as a token) and emit operator token */
		if (input[r.i] != '\0' && is_operator(input[r.i]))
		{
					if (r.tmp && r.tmp[0] != '\0')
		{
			r.token_quote = *last_quote;
			check_and_add_token(r.tokens, &r.tok_i, &r.tmp);
			(*quotes_out)[r.tok_i - 1] = r.token_quote;
			*last_quote = QUOTE_NONE;
		}
		else if (r.tmp)
		{
			free(r.tmp);  // Free empty string
			r.tmp = NULL;
		}
			{
				int		len = operator_len(&input[r.i]);
				char	*op = ft_substr(input, r.i, len);
				add_token(r.tokens, &r.tok_i, &op);
				(*quotes_out)[r.tok_i - 1] = QUOTE_NONE;
				r.i += len;
			}
			continue;
		}
		/* Add token when:
		 * - we're at end or space boundary, AND
		 * - tmp is non-empty OR it is empty but came from quotes ("" or '') */
		if (r.tmp && r.tmp[0] != '\0' && (input[r.i] == '\0' || ft_isspace((unsigned char)input[r.i])))
		{
			r.token_quote = *last_quote;
			check_and_add_token(r.tokens, &r.tok_i, &r.tmp);
			(*quotes_out)[r.tok_i - 1] = r.token_quote;
			/* reset quote context after emitting a token */
			*last_quote = QUOTE_NONE;
		}
		else if (r.tmp && (input[r.i] == '\0' || ft_isspace((unsigned char)input[r.i])))
		{
			free(r.tmp);  // Free empty string
			r.tmp = NULL;
		}
		if (r.last_i == r.i)
			r.i++;
	}
	r.tokens[r.tok_i] = NULL;
	(*quotes_out)[r.tok_i] = QUOTE_NONE;
	return (r.tokens);
}
