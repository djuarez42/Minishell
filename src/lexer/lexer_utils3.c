/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/03 18:52:03 by djuarez          ###   ########.fr       */
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

void print_clean_tokens_with_fragments(t_token *tokens)
{
    int i = 0;
    printf("=== CLEAN TOKENS ===\n");
    while (tokens)
    {
        printf("Token %d: type=%d has_space_before=%d\n", i, tokens->type, tokens->has_space_before);

        t_fragment *frag = tokens->fragments;
        int j = 0;
        while (frag)
        {
            const char *qtype_str = "NONE";
            if (frag->quote_type == QUOTE_SINGLE) qtype_str = "SINGLE";
            if (frag->quote_type == QUOTE_DOUBLE) qtype_str = "DOUBLE";

            printf("    Fragment %d: text=\"%s\" quote=%s has_space_after=%d\n",
                   j, frag->text, qtype_str, frag->has_space_after);

            frag = frag->next;
            j++;
        }

        tokens = tokens->next;
        i++;
    }
    printf("=======================================\n");
}