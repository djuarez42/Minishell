/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 00:22:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 17:55:49 by djuarez          ###   ########.fr       */
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

char	*concat_fragments(t_fragment *frag)
{
	size_t	len;
	char	*res;

	if (!frag)
		return (NULL);
	len = calc_total_length(frag);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	copy_fragments_to_buffer(frag, res);
	return (res);
}

t_token	*append_token_eof(t_token *head)
{
	t_token	*new;
	t_token	*cur;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->fragments = NULL;
	new->final_text = NULL;
	new->type = TOKEN_EOF;
	new->has_space_before = false;
	new->next = NULL;
	if (!head)
		return (new);
	cur = head;
	while (cur->next)
		cur = cur->next;
	cur->next = new;
	return (head);
}
