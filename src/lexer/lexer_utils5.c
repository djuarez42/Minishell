/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 00:22:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 22:50:32 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token *build_token_list_from_fragments(t_token *raw)
{
    if (!raw)
        return NULL;

    t_token *head = NULL;
    t_token *current_token = NULL;
    t_token *prev_token = NULL; // para enlazar tokens
    bool next_space_before = false;

    for (t_token *raw_tok = raw; raw_tok; raw_tok = raw_tok->next)
    {
        t_fragment *frag = raw_tok->fragments;

        while (frag)
        {
            bool empty_quote = (strlen(frag->text) == 0 && frag->quote_type != QUOTE_NONE);

            printf("Processing fragment: \"%s\" quote=%d has_space_after=%d\n",
                   frag->text, frag->quote_type, frag->has_space_after);
            printf("  Current token: %p, next_space_before=%d\n",
                   (void*)current_token, next_space_before);

            if (!current_token)
            {
                if (!empty_quote)
                {
                    t_fragment *frag_copy = duplicate_fragment(frag);
                    current_token = create_token_from_fragments(frag_copy, next_space_before);

                    if (!head)
                        head = current_token;

                    if (prev_token)
                        prev_token->next = current_token;

                    prev_token = current_token;
                    next_space_before = false;

                    printf("  Created new token %p with fragment \"%s\" has_space_before=%d\n",
                           (void*)current_token, frag->text, current_token->has_space_before);
                }
                else if (frag->has_space_after)
                {
                    next_space_before = true; // token siguiente tendrá espacio antes
                }
            }
            else
            {
                if (!empty_quote)
                {
                    t_fragment *frag_copy = duplicate_fragment(frag);
                    append_fragment(&current_token->fragments, frag_copy);

                    printf("  Appended fragment \"%s\" to current token %p\n",
                           frag->text, (void*)current_token);
                }
            }

            if (frag->has_space_after)
            {
                current_token = NULL;
                next_space_before = true;
                printf("  Fragment has space after → closing current token\n");
            }

            frag = frag->next;
        }
    }

    return head;
}


bool lx_is_space_between(t_fragment *cur, t_fragment *next)
{
    if (!cur || !next)
        return false;

    // Si hay espacio real después del fragmento anterior, siempre romper
    if (cur->has_space_after)
        return true;

    // Ambos sin comillas: romper solo si hay espacio
    if (cur->quote_type == QUOTE_NONE && next->quote_type == QUOTE_NONE)
        return false; // el espacio real ya se capturó en cur->has_space_after

    // Uno con quote y otro sin → romper
    if ((cur->quote_type == QUOTE_NONE && next->quote_type != QUOTE_NONE) ||
        (cur->quote_type != QUOTE_NONE && next->quote_type == QUOTE_NONE))
        return true;

    // Misma quote → no romper
    if (cur->quote_type == next->quote_type)
        return false;

    // Quotes diferentes → romper
    return true;
}

char *concat_fragments(t_fragment *frag)
{
    if (!frag) return NULL;

    size_t len = 0;
    t_fragment *cur = frag;
    while (cur)
    {
        len += strlen(cur->text);
        cur = cur->next;
    }

    char *res = malloc(len + 1);
    if (!res) return NULL;

    size_t pos = 0;
    cur = frag;
    while (cur)
    {
        size_t i = 0;
        while (cur->text[i])
            res[pos++] = cur->text[i++];
        cur = cur->next;
    }
    res[pos] = '\0';
    return res;
}

t_fragment *duplicate_fragment(t_fragment *frag)
{
    if (!frag) return NULL;

    t_fragment *copy = malloc(sizeof(t_fragment));
    if (!copy) return NULL;

    copy->text = strdup(frag->text);
    if (!copy->text) { free(copy); return NULL; }

    copy->quote_type = frag->quote_type;
    copy->has_space_after = frag->has_space_after;
    copy->next = NULL;

    return copy;
}