/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 19:20:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/06 20:25:07 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

t_token	*tokenize_input(const char *input)
{
	char	**tokens;
	t_token	*token_list;

	if (!input)
		return (NULL);
	tokens = clean_input_quotes(input);
	if (!tokens)
		return (NULL);
	token_list = build_token_list(tokens);
	free_tokens(tokens, -1);
	return (token_list);
}