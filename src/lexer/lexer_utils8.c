/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils8.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 21:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/02 17:14:30 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	copy_fragments_to_buffer(t_fragment *frag, char *res)
{
	size_t		pos;
	size_t		i;
	t_fragment	*cur;

	pos = 0;
	cur = frag;
	while (cur)
	{
		if (cur->text)
		{
			i = 0;
			while (cur->text[i])
			{
				res[pos] = cur->text[i];
				pos++;
				i++;
			}
		}
		cur = cur->next;
	}
	res[pos] = '\0';
}

void	append_char_to_buf(char **buf, char c)
{
	char	*tmp;
	size_t	len;

	len = 0;
	if (*buf)
		len = ft_strlen(*buf);
	tmp = malloc(len + 2);
	if (!tmp)
		return ;
	if (*buf)
	{
		ft_strlcpy(tmp, *buf, len + 1);
		free(*buf);
	}
	tmp[len] = c;
	tmp[len + 1] = '\0';
	*buf = tmp;
}

char	*collect_double_quote_text(const char *text, int *i)
{
	char	*buf;
	/* local variables used previously for parity handling removed */

	buf = NULL;
	while (text[*i] && text[*i] != '"')
	{
		if (text[*i] == '\\')
		{
			/* simple escape handling: if next char exists, append it
			   (backslash consumes next char inside double quotes for \", \\, ` and $ is handled later) */
			if (text[*i + 1])
			{
				/* if next is a special escapable char, append that char */
				if (text[*i + 1] == '"' || text[*i + 1] == '\\' || text[*i + 1] == '`')
				{
					append_char_to_buf(&buf, text[*i + 1]);
					*i += 2;
					continue;
				}
				/* otherwise keep the backslash as literal */
				append_char_to_buf(&buf, '\\');
				(*i)++;
				continue;
			}
			/* trailing backslash at end */
			append_char_to_buf(&buf, '\\');
			(*i)++;
			continue;
		}
		append_char_to_buf(&buf, text[*i]);
		(*i)++;
	}
	return (buf);
}

bool	compute_space_after(const char *text, int i)
{
	if (text[i + 1])
	{
		if (ft_isspace((unsigned char)text[i + 1]))
			return (true);
	}
	return (false);
}
