/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 21:10:57 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 20:22:24 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token *tokenize_input(const char *input) 
{
    t_fragment *frags = parse_fragments(input);
    t_fragment *cur = frags;
    t_token *tokens = NULL;
    bool space_before = false;

    while (cur) {
        // Saltar fragments vacíos que no sean quotes
        while (cur && strlen(cur->text) == 0 && cur->quote_type == QUOTE_NONE)
            cur = cur->next;
        if (!cur) break;

        t_token *tok = create_token(TOKEN_WORD, space_before);
        space_before = false;

        t_fragment *last_frag = NULL;

        while (cur) {
            // Añadir fragment
            t_fragment *frag_copy = new_fragment(cur->text, strlen(cur->text), cur->quote_type, cur->has_space_after);
            append_fragment(&tok->fragments, frag_copy);


            last_frag = cur;
            cur = cur->next;

            if (last_frag->has_space_after) {
                space_before = true;
                break; // nuevo token después del espacio
            }
        }

        append_token(&tokens, tok);
    }

    return tokens;
}
