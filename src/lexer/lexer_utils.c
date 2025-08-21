/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 11:03:33 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/21 18:55:36 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

int	is_blank(const char *s)
{
	while (*s)
	{
		if (*s != ' ')
			return (0);
		s++;
	}
	return (1);
}

void	free_tokens(char **tokens, int max)
{
	int	i;

	if (!tokens)
		return ;
	i = 0;
	if (max < 0)
	{
		while (tokens[i])
		{
			free(tokens[i]);
			tokens[i] = NULL;
			i++;
		}
	}
	else
	{
		while (i < max && tokens[i])
		{
			free(tokens[i]);
			tokens[i] = NULL;
			i++;
		}
	}
	free(tokens);
}

const char	*quote_type_str(t_quote_type q)
{
	if (q == QUOTE_NONE)
		return ("NONE");
	else if (q == QUOTE_SINGLE)
		return ("SINGLE");
	else if (q == QUOTE_DOUBLE)
		return ("DOUBLE");
	return ("UNKNOWN");
}

char	**clean_input_quotes(const char *input, t_quote_type **quotes_out)
{
	char			*comment_free_input;
	char			**result;
	t_quote_type	last_quote;

	if (!input)
		return (NULL);
	comment_free_input = strip_comments(input);
	if (!comment_free_input)
		return (NULL);
	last_quote = QUOTE_NONE;
	if (!are_quotes_closed(comment_free_input))
	{
		free(comment_free_input);
		return (NULL);
	}
	result = reconstruct_words(comment_free_input, &last_quote, quotes_out);
	free(comment_free_input);
	return (result);
}
