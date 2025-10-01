/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 00:22:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/01 17:57:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_unmatched_quotes(const char *input)
{
	int	single_open;
	int	double_open;
	int	i;

	single_open = 0;
	double_open = 0;
	i = 0;
	while (input[i])
	{
		if (input[i] == '\'' && double_open == 0)
			single_open = !single_open;
		else if (input[i] == '"' && single_open == 0)
			double_open = !double_open;
		i++;
	}
	if (single_open || double_open)
		return (1);
	return (0);
}

t_fragment	*handle_backslashes_even_dollar(int keep, const char *text, int *i)
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

t_fragment	*handle_backslashes_odd_dollar(int keep, const char *text, int *i)
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

char	*reduce_backslashes_literal(int count, char next_char)
{
	char	*buf;
	int		i;
	int		keep;
	int		bufsize;

	if (next_char == '\0')
		keep = count;
	else
		keep = count / 2;
	bufsize = keep + 1;
	if (count % 2 != 0 && next_char != '\0')
		bufsize++;
	buf = malloc((size_t)bufsize);
	if (!buf)
		return (NULL);
	i = 0;
	while (i < keep)
		buf[i++] = '\\';
	if (count % 2 != 0 && next_char != '\0')
		buf[i++] = next_char;
	buf[i] = '\0';
	return (buf);
}

t_fragment	*handle_backslashes_literal(int count, const char *text, int *i)
{
	char		*buf;
	bool		space_after;
	t_fragment	*f;
	char		next_char;

	next_char = text[*i];
	buf = reduce_backslashes_literal(count, next_char);
	if (!buf)
		return (NULL);
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	f = new_fragment(buf, ft_strlen(buf), QUOTE_NONE, space_after);
	free(buf);
	if (count % 2 != 0 && next_char != '\0')
		(*i)++;
	return (f);
}
