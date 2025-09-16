/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 18:10:18 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_consecutive_backslashes(const char *text, int *i)
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

t_fragment	*handle_backslashes_dispatch(int count, const char *text,
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
