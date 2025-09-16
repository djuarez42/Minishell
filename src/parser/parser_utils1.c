/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 19:08:13 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 19:32:35 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_words_spaces(const char *s)
{
	int			count;
	int			in_word;
	const char	*p;

	count = 0;
	in_word = 0;
	p = s;
	while (*p)
	{
		if (!isspace((unsigned char)*p))
		{
			if (!in_word)
			{
				count++;
				in_word = 1;
			}
		}
		else
			in_word = 0;
		p++;
	}
	return (count);
}

static void	copy_words_spaces(char **res, const char *s)
{
	int		w;
	int		len;

	w = 0;
	while (*s)
	{
		while (*s && isspace((unsigned char)*s))
			s++;
		if (!*s)
			break ;
		len = 0;
		while (s[len] && !isspace((unsigned char)s[len]))
			len++;
		res[w++] = ft_strndup(s, len);
		s += len;
	}
	res[w] = NULL;
}

char	**ft_split_spaces(const char *s)
{
	char	**res;
	int		count;

	if (!s)
		return (NULL);
	count = count_words_spaces(s);
	res = malloc(sizeof(char *) * (count + 1));
	if (!res)
		return (NULL);
	copy_words_spaces(res, s);
	return (res);
}

static t_quote_type	update_result_from_quote(t_quote_type current,
						t_quote_type result)
{
	if (current == QUOTE_SINGLE)
	{
		if (result == QUOTE_NONE)
			return (QUOTE_SINGLE);
		else if (result != QUOTE_SINGLE)
			return (QUOTE_MIXED);
	}
	else if (current == QUOTE_DOUBLE)
	{
		if (result == QUOTE_NONE)
			return (QUOTE_DOUBLE);
		else if (result != QUOTE_DOUBLE)
			return (QUOTE_MIXED);
	}
	else if (current == QUOTE_DOLLAR)
		return (QUOTE_DOLLAR);
	else if (current == QUOTE_MIXED)
		return (QUOTE_MIXED);
	return (result);
}

t_quote_type	detect_combined_quote(t_fragment *frags)
{
	t_quote_type	result;
	t_fragment		*cur;
	t_quote_type	updated;

	result = QUOTE_NONE;
	cur = frags;
	while (cur)
	{
		updated = update_result_from_quote(cur->quote_type, result);
		if (updated != result && (updated == QUOTE_MIXED
				|| updated == QUOTE_DOLLAR))
			return (updated);
		result = updated;
		cur = cur->next;
	}
	return (result);
}
