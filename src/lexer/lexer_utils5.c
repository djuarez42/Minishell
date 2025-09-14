/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 00:22:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 01:39:21 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *concat_fragments(t_fragment *frag)
{
    if (!frag)
        return NULL;

    size_t len = 0;
    t_fragment *cur = frag;
    while (cur)
    {
        if (cur->text)
            len += strlen(cur->text);
        cur = cur->next;
    }

    char *res = malloc(len + 1);
    if (!res)
        return NULL;

    size_t pos = 0;
    cur = frag;
    while (cur)
    {
        if (cur->text)
        {
            size_t i = 0;
            while (cur->text[i])
                res[pos++] = cur->text[i++];
        }
        cur = cur->next;
    }
    res[pos] = '\0';
    return res;
}

t_token *append_token_eof(t_token *head)

{
    t_token *new;

    new = malloc(sizeof(t_token));
    if (!new)
        return NULL;

    new->fragments = NULL;
    new->final_text = NULL;
    new->type = TOKEN_EOF;
    new->has_space_before = false;
    new->next = NULL;

    if (!head)
        return new;

    // Encuentra el último token de la lista
    t_token *cur = head;
    while (cur->next)
        cur = cur->next;

    cur->next = new;
    return head;
}
