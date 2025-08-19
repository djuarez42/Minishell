/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 19:20:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/17 20:23:52 by djuarez          ###   ########.fr       */
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
