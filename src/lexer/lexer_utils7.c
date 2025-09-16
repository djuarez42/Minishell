/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils7.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:14:41 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 18:16:34 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	calc_total_length(t_fragment *frag)
{
	size_t		len;
	t_fragment	*cur;

	len = 0;
	cur = frag;
	while (cur)
	{
		if (cur->text)
			len += strlen(cur->text);
		cur = cur->next;
	}
	return (len);
}

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
