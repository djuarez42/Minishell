/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/01 17:30:17 by djuarez          ###   ########.fr       */
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

t_fragment	*handle_backslashes_dispatch(int count, const char *text, int *i)
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
	return (handle_backslashes_literal(count, text, i));
}


t_fragment	*handle_backslashes(const char *text, int *i)
{
	int	count;

	count = count_consecutive_backslashes(text, i);
	return (handle_backslashes_dispatch(count, text, i));
}

t_fragment	*handle_spaces(const char *text, int *i)
{
	while (text[*i] && ft_isspace((unsigned char)text[*i]))
		(*i)++;
	return (NULL);
}

size_t	calc_total_length(t_fragment *frag)
{
	size_t		len;
	t_fragment	*cur;

	len = 0;
	cur = frag;
	while (cur)
	{
		if (cur->text)
			len += ft_strlen(cur->text);
		cur = cur->next;
	}
	return (len);
}
