/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 00:50:22 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_fragments(t_fragment *frag)
{
    t_fragment *tmp;
    while (frag)
    {
        tmp = frag->next;
        free(frag->text);
        free(frag);
        frag = tmp;
    }
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		if (tokens->fragments)
			free_fragments(tokens->fragments);
		free(tokens);
		tokens = tmp;
	}
}

void print_token_list_from_fragments(t_token *tokens)
{
    int i = 0;
    while (tokens)
    {
        printf("Token %d: type=%d\n", i, tokens->type);
        t_fragment *frag = tokens->fragments;
        while (frag)
        {
            printf("  Fragment text=\"%s\" quote=%d\n", frag->text, frag->quote_type);
            frag = frag->next;
        }
        tokens = tokens->next;
        i++;
    }
}
