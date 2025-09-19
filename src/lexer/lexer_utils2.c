/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:45:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/18 20:59:26 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_fragment	*handle_double_quotes(const char *text, int *i)
{
	int			start;
	int			len;
	bool		space_after;
	t_fragment	*frag;

	(*i)++;
	start = *i;
	while (text[*i] && text[*i] != '"')
	{
		if (text[*i] == '\\' && text[*i + 1])
			*i += 2;
		else
			(*i)++;
	}
	len = *i - start;
	space_after = text[*i + 1] && ft_isspace((unsigned char)text[*i + 1]);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_DOUBLE, space_after);
	if (text[*i] == '"')
		(*i)++;
	return (frag);
}

t_fragment	*handle_operators(const char *text, int *i)
{
	int			start;
	int			len;
	bool		space_after;

	start = *i;
	if (text[*i] == '<' && text[*i + 1] == '<')
	{
		*i += 2;
	}
	else if (text[*i] == '>' && text[*i + 1] == '>')
	{
		*i += 2;
	}
	else
	{
		(*i)++;
	}
	len = *i - start;
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	return (new_fragment(&text[start], (size_t)len, QUOTE_NONE, space_after));
}

t_fragment	*handle_generic_text(const char *text, int *i)
{
	t_fragment	*frag;
	int			start;
	int			len;
	bool		space_after;

	frag = NULL;
	start = *i;
	while (text[*i] && !ft_isspace((unsigned char)text[*i]) && text[*i] != '\''
		&& text[*i] != '"' && text[*i] != '|'
		&& text[*i] != '<' && text[*i] != '>')
	{
		(*i)++;
	}
	len = *i - start;
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_NONE, space_after);
	return (frag);
}

t_fragment	*get_next_fragment(const char *text, int *i)
{
	t_fragment	*frag;

	frag = NULL;
	if (text[*i] == '\\')
		frag = handle_backslashes_wrapper(text, i);
	else if (is_dollar_string(text, *i))
		frag = handle_dollar_string_lexer(text, i);
	else if (text[*i] == '\'')
		frag = handle_single_quotes(text, i);
	else if (text[*i] == '"')
		frag = handle_double_quotes(text, i);
	else if (text[*i] == '|' || text[*i] == '<' || text[*i] == '>')
		frag = handle_operators(text, i);
	else
		frag = handle_generic_text(text, i);
	return (frag);
}

t_fragment	*parse_mixed_fragments(const char *text)
{
	t_fragment	*fragments;
	t_fragment	*frag;
	int			i;

	i = 0;
	fragments = NULL;
	while (text[i])
	{
		handle_spaces(text, &i);
		if (!text[i])
			break ;
		frag = get_next_fragment(text, &i);
		if (frag)
			append_fragment(&fragments, frag);
	}
	//print_fragments(fragments);
	return (fragments);
}
