/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/02 19:34:55 by djuarez          ###   ########.fr       */
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

void	free_token_list(t_token *tokens)
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
    
void print_final_token_list(t_token *tokens)
{
    int i = 0;
    printf("=== FINAL TOKEN LIST ===\n");
    while (tokens)
    {
        // concatenamos todos los fragments en una cadena final
        char *final_text = concat_fragments(tokens->fragments);

        printf("Token %d: text=\"%s\" type=%d has_space_before=%d\n",
               i, final_text ? final_text : "(NULL)",
               tokens->type, tokens->has_space_before);

        free(final_text);
        tokens = tokens->next;
        i++;
    }
    printf("=======================================\n\n");
}
