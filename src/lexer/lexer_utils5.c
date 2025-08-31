/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 00:22:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 04:09:53 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token *build_token_list_from_fragments(t_token *raw_tokens)
{
    t_token *head = NULL;
    t_token *last_tok = NULL;

    while (raw_tokens)
    {
        t_fragment *cur_frag = raw_tokens->fragments;

        while (cur_frag)
        {
            if (ft_strlen(cur_frag->text) == 0)
            {
                cur_frag = cur_frag->next;
                continue;
            }

            t_token *new_tok = malloc(sizeof(t_token));
            new_tok->type = TOKEN_WORD;
            new_tok->fragments = NULL;
            new_tok->next = NULL;

            t_fragment *frag_head = NULL;
            t_fragment *frag_last = NULL;

            while (cur_frag)
            {
                t_fragment *next_frag = cur_frag->next;

                if (frag_last && lx_is_space_between(frag_last, cur_frag))
                    break;

                t_fragment *copy = malloc(sizeof(t_fragment));
                copy->text = ft_strdup(cur_frag->text);
                copy->quote_type = cur_frag->quote_type;
                copy->next = NULL;

                if (!frag_head)
                    frag_head = copy;
                else
                    frag_last->next = copy;
                frag_last = copy;

                cur_frag = next_frag;
            }

            char *word = concat_fragments(frag_head);

            t_fragment *final_frag = malloc(sizeof(t_fragment));
            final_frag->text = word;
            final_frag->quote_type = (frag_head && frag_head->quote_type != QUOTE_NONE)
                                      ? frag_head->quote_type
                                      : QUOTE_NONE;
            final_frag->next = NULL;

            new_tok->fragments = final_frag;

            if (!head)
                head = new_tok;
            else
                last_tok->next = new_tok;
            last_tok = new_tok;

            // Si rompimos por espacio, cur_frag sigue apuntando al fragment para el siguiente token
        }

        raw_tokens = raw_tokens->next;
    }

    t_token *eof = malloc(sizeof(t_token));
    eof->type = TOKEN_EOF;
    eof->fragments = NULL;
    eof->next = NULL;

    if (!head)
        head = eof;
    else
        last_tok->next = eof;

    return head;
}

bool lx_is_space_between(t_fragment *cur, t_fragment *next)
{
    if (!cur || !next)
        return false;

    // Solo hay espacio entre fragments sin comillas
    if (cur->quote_type == QUOTE_NONE && next->quote_type == QUOTE_NONE)
        return true;

    return false;
}

char *concat_fragments(t_fragment *frag)
{
    if (!frag)
        return NULL;

    size_t len = 0;
    t_fragment *cur = frag;

    while (cur)
    {
        len += ft_strlen(cur->text);
        if (cur->next && cur->quote_type == QUOTE_NONE && cur->next->quote_type == QUOTE_NONE)
            len += 1; // añadir espacio
        cur = cur->next;
    }

    char *res = malloc(len + 1);
    size_t pos = 0;
    cur = frag;

    while (cur)
    {
        size_t i = 0;
        while (cur->text[i])
            res[pos++] = cur->text[i++];

        if (cur->next && cur->quote_type == QUOTE_NONE && cur->next->quote_type == QUOTE_NONE)
            res[pos++] = ' ';
        cur = cur->next;
    }
    res[pos] = '\0';

    return res;
}