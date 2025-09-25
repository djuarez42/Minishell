/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 00:22:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/25 21:11:55 by ekakhmad         ###   ########.fr       */
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
	{
		/* Do not print here; caller will print exactly once and set exit status */
		return (1);
	}
	return (0);
}

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

t_fragment	*handle_backslashes_literal(int start, int count, const char *text,
		int *i)
{
	bool		space_after;
	t_fragment	*f;

	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	f = new_fragment(&text[start], (size_t)count, QUOTE_NONE, space_after);
	return (f);
}
