/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 20:06:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 21:15:36 by ekakhmad         ###   ########.fr       */
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
		while (i < wb->len && ft_isspace((unsigned char)wb->buf[i])
			&& wb->splittable[i])
			i++;
		if (i >= wb->len)
			break ;
		wb->count++;
		while (i < wb->len && !(ft_isspace((unsigned char)wb->buf[i])
				&& wb->splittable[i]))
			i++;
	}
}

static int	count_quoted_groups(t_fragment *frag)
{
	int			count;
	int			in_group;
	t_fragment	*cur;

	count = 0;
	in_group = 0;
	cur = frag;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE || cur->quote_type == QUOTE_DOUBLE)
		{
			if (!in_group)
			{
				count++;
				in_group = 1;
			}
		}
		if (cur->has_space_after)
			in_group = 0;
		cur = cur->next;
	}
	return (count);
}

static void	fill_quoted_groups(t_fragment *frag, char **words)
{
	int			wi;
	int			in_group;
	t_fragment	*cur;

	wi = 0;
	in_group = 0;
	cur = frag;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE || cur->quote_type == QUOTE_DOUBLE)
		{
			if (!in_group)
			{
				words[wi++] = ft_strdup("");
				in_group = 1;
			}
			if (cur->has_space_after)
				in_group = 0;
		}
		if (cur->has_space_after)
			in_group = 0;
		cur = cur->next;
	}
}

char	**build_from_quoted_fragments(t_fragment *frag, int *out_count)
{
	char	**words;
	int		groups;

	groups = count_quoted_groups(frag);
	if (groups == 0)
	{
		*out_count = 0;
		return (NULL);
	}
	words = malloc(sizeof(char *) * (groups + 1));
	if (!words)
	{
		*out_count = 0;
		return (NULL);
	}
	fill_quoted_groups(frag, words);
	words[groups] = NULL;
	*out_count = groups;
	return (words);
}
