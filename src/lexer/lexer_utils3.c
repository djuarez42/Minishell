/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/05 19:05:50 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	skip_until_closing_quote(const char *text, int *i, char quote)
{
	while (text[*i] && text[*i] != quote)
	{
		if (quote == '"' && text[*i] == '\\' && text[*i + 1])
			*i += 2;
		else
			(*i)++;
	}
}

bool	calc_space_after(const char *text, int i)
{
	return (text[i + 1] && ft_isspace((unsigned char)text[i + 1]));
}

t_fragment	*make_dollar_fragment(const char *text, int start,
			int len, char quote)
{
	bool		space_after;
	t_fragment	*frag;

	space_after = calc_space_after(text, start + len - 1);
	if (quote == '\'')
		frag = new_fragment(&text[start], (size_t)len,
				QUOTE_DOLLAR, space_after);
	else
		frag = new_fragment(&text[start], (size_t)len,
				QUOTE_DOUBLE, space_after);
	return (frag);
}

t_fragment	*handle_dollar_string_lexer(const char *text, int *i)
{
	char		quote;
	int			start;
	int			len;
	t_fragment	*frag;

	quote = text[*i + 1];
	*i += 2;
	start = *i;
	skip_until_closing_quote(text, i, quote);
	len = *i - start;
	frag = make_dollar_fragment(text, start, len, quote);
	if (text[*i] == quote)
		(*i)++;
	return (frag);
}

t_fragment	*handle_single_quotes(const char *text, int *i)
{
	int			start;
	int			len;
	bool		space_after;
	t_fragment	*frag;

	(*i)++;
	start = *i;
	while (text[*i] && text[*i] != '\'')
		(*i)++;
	len = *i - start;
	space_after = calc_space_after(text, *i);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_SINGLE, space_after);
	if (text[*i] == '\'')
		(*i)++;
	return (frag);
}
