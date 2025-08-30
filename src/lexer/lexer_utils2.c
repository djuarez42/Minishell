/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:45:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 18:08:58 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_fragment *new_fragment(const char *text, int len, t_quote_type qtype)
{
    t_fragment *frag;

    frag = malloc(sizeof(t_fragment));
    if (!frag)
        return (NULL);
    frag->text = ft_substr(text, 0, len);
    if (!frag->text)
    {
        free(frag);
        return (NULL);
    }
    frag->quote_type = qtype;
    frag->next = NULL;
    return (frag);
}

void append_fragment(t_fragment **head, t_fragment *frag)
{
    t_fragment *tmp;

    if (!*head)
    {
        *head = frag;
        return;
    }
    tmp = *head;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = frag;
}

t_fragment *parse_fragments(const char *text)
{
    t_fragment *fragments = NULL;
    int i = 0;
    int start;
    t_quote_type qtype;

    while (text[i])
    {
        // Saltar espacios
        while (text[i] && ft_isspace((unsigned char)text[i]))
            i++;
        if (!text[i])
            break;

        start = i;
        qtype = QUOTE_NONE;

        if (text[i] == '\'') // Comillas simples
        {
            qtype = QUOTE_SINGLE;
            i++; // Saltar la comilla inicial
            start = i;
            while (text[i] && text[i] != '\'')
                i++;
            if (i > start) // Evitar fragmentos vacíos
                append_fragment(&fragments, new_fragment(&text[start], i - start, qtype));
            if (text[i] == '\'')
                i++; // Saltar la comilla final
        }
        else if (text[i] == '"') // Comillas dobles
        {
            qtype = QUOTE_DOUBLE;
            i++; // Saltar la comilla inicial
            start = i;
            while (text[i] && text[i] != '"')
            {
                if (text[i] == '\\' && text[i + 1])
                    i += 2;
                else
                    i++;
            }
            if (i > start) // Evitar fragmentos vacíos
                append_fragment(&fragments, new_fragment(&text[start], i - start, qtype));
            if (text[i] == '"')
                i++; // Saltar la comilla final
        }
        else // Texto normal fuera de comillas
        {
            start = i;
            while (text[i] && !ft_isspace((unsigned char)text[i])
                   && text[i] != '\'' && text[i] != '"')
                i++;
            if (i > start)
                append_fragment(&fragments, new_fragment(&text[start], i - start, QUOTE_NONE));
        }
    }
    return fragments;
}


