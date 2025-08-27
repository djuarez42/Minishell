/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 19:20:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/27 20:32:42 by djuarez          ###   ########.fr       */
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
		while (ft_isspace(input[r.i]))
			r.i++;
		if (!input[r.i])
			break ;

		if (is_quote(input[r.i]))
		{
			r.tmp = handle_quoted_part(input, &r.i, r.tmp, last_quote);
			// cada segmento con comillas se agrega como un token separado
			r.tokens[r.tok_i] = r.tmp;
			(*quotes_out)[r.tok_i] = *last_quote;
			r.tok_i++;
			r.tmp = NULL;
			*last_quote = QUOTE_NONE;
		}
		else
		{
			r.tmp = handle_plain_text(input, &r.i, r.tmp);
			r.tokens[r.tok_i] = r.tmp;
			(*quotes_out)[r.tok_i] = QUOTE_NONE;
			r.tok_i++;
			r.tmp = NULL;
		}
	}

	r.tokens[r.tok_i] = NULL;
	(*quotes_out)[r.tok_i] = QUOTE_NONE;
	return (r.tokens);
}
