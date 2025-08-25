/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 19:20:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/25 20:20:24 by ekakhmad         ###   ########.fr       */
=======
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 19:20:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/25 20:49:17 by ekakhmad         ###   ########.fr       */
>>>>>>> 78360fa27b4289310631088017a384aa179b923b
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
		r.last_i = r.i;
		r.i = process_spaces_and_quotes(input, r.i, &r.tmp, last_quote);
		if (r.i == -1)
			break ;
		/* Add token when:
		 * - we're at end or space boundary, AND
		 * - tmp is non-empty OR it is empty but came from quotes ("" or '') */
		if (r.tmp && (input[r.i] == '\0' || ft_isspace((unsigned char)input[r.i])))
		{
			r.token_quote = *last_quote;
			check_and_add_token(r.tokens, &r.tok_i, &r.tmp);
			(*quotes_out)[r.tok_i - 1] = r.token_quote;
			/* reset quote context after emitting a token */
			*last_quote = QUOTE_NONE;
		}
		if (r.last_i == r.i)
			r.i++;
	}
	r.tokens[r.tok_i] = NULL;
	(*quotes_out)[r.tok_i] = QUOTE_NONE;
	return (r.tokens);
}
