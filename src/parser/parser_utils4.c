/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 20:06:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 20:48:07 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	fill_buffer_and_splittable(t_fragment *frag, t_word_builder *wb)
{
	wb->len = 0;
	while (frag)
	{
		if (frag->expanded_text)
			copy_fragment_to_buffer(frag, wb);
		frag = frag->next;
	}
	wb->buf[wb->len] = '\0';
}

void	count_words(t_word_builder *wb)
{
	size_t	i;

	i = 0;
	wb->count = 0;
	while (i < wb->len)
	{
		while (i < wb->len
			&& ft_isspace((unsigned char)wb->buf[i])
			&& wb->splittable[i])
			i++;
		if (i >= wb->len)
			break ;
		wb->count++;
		while (i < wb->len
			&& !(ft_isspace((unsigned char)wb->buf[i])
				&& wb->splittable[i]))
			i++;
	}
}

int	count_quoted_fragments(t_fragment *frag)
{
	t_fragment	*cur;
	int			count;

	cur = frag;
	count = 0;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE
			|| cur->quote_type == QUOTE_DOUBLE)
			count++;
		cur = cur->next;
	}
	return (count);
}

void	fill_quoted_words(t_fragment *frag, char **words)
{
	t_fragment	*cur;
	int			i;

	cur = frag;
	i = 0;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE
			|| cur->quote_type == QUOTE_DOUBLE)
		{
			words[i] = ft_strdup("");
			i++;
		}
		cur = cur->next;
	}
}

char	**build_from_quoted_fragments(t_fragment *frag, int *out_count)
{
	char	**words;
	int		frag_count;

	frag_count = count_quoted_fragments(frag);
	if (frag_count == 0)
	{
		*out_count = 0;
		return (NULL);
	}
	words = malloc(sizeof(char *) * (frag_count + 1));
	if (!words)
	{
		*out_count = 0;
		return (NULL);
	}
	fill_quoted_words(frag, words);
	words[frag_count] = NULL;
	*out_count = frag_count;
	return (words);
}
