/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:45:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 22:24:41 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	append_fragment(t_fragment **head, t_fragment *frag)
{
	t_fragment	*tmp;

	tmp = *head;
	if (!frag)
		return ;
	if (!*head)
	{
		*head = frag;
		return ;
	}
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = frag;
}

static t_fragment	*handle_backslashes_even_dollar(int keep, const char
					*text, int *i)
{
	int			j;
	char		*buf;
	bool		space_after;
	t_fragment	*f;

	j = 0;
	buf = malloc((size_t)keep + 1);
	if (!buf)
		return (NULL);
	while (j < keep)
	{
		buf[j] = '\\';
		j++;
	}
	buf[keep] = '\0';
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	f = new_fragment(buf, keep, QUOTE_NONE, space_after);
	free(buf);
	return (f);
}

static t_fragment	*handle_backslashes_odd_dollar(int keep, const char
					*text, int *i)
{
	int			j;
	int			buflen;
	char		*buf;
	bool		space_after;
	t_fragment	*f;

	j = 0;
	buflen = keep + 1;
	buf = malloc((size_t)buflen + 1);
	if (!buf)
		return (NULL);
	while (j < keep)
	{
		buf[j] = '\\';
		j++;
	}
	buf[keep] = '$';
	buf[buflen] = '\0';
	(*i)++;
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	f = new_fragment(buf, buflen, QUOTE_NONE, space_after);
	free(buf);
	return (f);
}

static t_fragment	*handle_backslashes_literal(int start, int count,
				const char *text, int *i)
{
	bool		space_after;
	t_fragment	*f;

	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	f = new_fragment(&text[start], (size_t)count, QUOTE_NONE, space_after);
	return (f);
}

static int	count_consecutive_backslashes(const char *text, int *i)
{
	int	count;

	count = 0;
	while (text[*i] && text[*i] == '\\')
	{
		count++;
		(*i)++;
	}
	return (count);
}

static t_fragment	*handle_backslashes_dispatch(int count, const char *text,
					int *i, int start)
{
	int		half;
	char	next;

	if (count == 0)
		return (NULL);
	next = text[*i];
	if (next == '$')
	{
		half = count / 2;
		if (count % 2 == 0)
		{
			if (half == 0)
				return (NULL);
			return (handle_backslashes_even_dollar(half, text, i));
		}
		return (handle_backslashes_odd_dollar(half, text, i));
	}
	return (handle_backslashes_literal(start, count, text, i));
}

t_fragment	*handle_backslashes(const char *text, int *i)
{
	int	start;
	int	count;

	start = *i;
	count = count_consecutive_backslashes(text, i);
	return (handle_backslashes_dispatch(count, text, i, start));
}

t_fragment	*handle_spaces(const char *text, int *i)
{
	while (text[*i] && ft_isspace((unsigned char)text[*i]))
		(*i)++;
	return (NULL);
}

t_fragment	*handle_backslashes_wrapper(const char *text, int *i)
{
	return (handle_backslashes(text, i));
}

static void	skip_until_closing_quote(const char *text, int *i, char quote)
{
	while (text[*i] && text[*i] != quote)
	{
		if (quote == '"' && text[*i] == '\\' && text[*i + 1])
			*i += 2;
		else
			(*i)++;
	}
}

static t_fragment	*make_dollar_fragment(const char *text, int start, int len,
					char quote, int i)
{
	bool		space_after;
	t_fragment	*frag;

	space_after = text[i + 1] && ft_isspace((unsigned char)text[i + 1]);
	if (quote == '\'')
		frag = new_fragment(&text[start], (size_t)len,
				QUOTE_DOLLAR, space_after);
	else
		frag = new_fragment(&text[start], (size_t)len,
				QUOTE_DOUBLE, space_after);
	return (frag);
}

t_fragment	*handle_dollar_string(const char *text, int *i)
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
	frag = make_dollar_fragment(text, start, len, quote, *i);
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
	space_after = text[*i + 1] && ft_isspace((unsigned char)text[*i + 1]);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_SINGLE, space_after);
	if (text[*i] == '\'')
		(*i)++;
	return (frag);
}

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

t_fragment *handle_generic_text(const char *text, int *i)
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

static t_fragment	*get_next_fragment(const char *text, int *i)
{
	t_fragment	*frag;

	frag = NULL;
	if (text[*i] == '\\')
		frag = handle_backslashes_wrapper(text, i);
	else if (is_dollar_string(text, *i))
		frag = handle_dollar_string(text, i);
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
	return (fragments);
}
