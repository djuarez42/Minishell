/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:45:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/03 17:02:45 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_fragment *new_fragment(const char *start, size_t len, t_quote_type qtype, bool space_after) 
{
    t_fragment *frag = malloc(sizeof(t_fragment));
    if (!frag) return NULL;
    frag->text = malloc(len + 1);
    if (!frag->text) { free(frag); return NULL; }
    memcpy(frag->text, start, len);
    frag->text[len] = '\0';
    frag->quote_type = qtype;
    frag->has_space_after = space_after;
    frag->next = NULL;

    return frag;
}


void append_fragment(t_fragment **head, t_fragment *frag)
{
    if (!frag) return;
    if (!*head) { *head = frag; return; }
    t_fragment *tmp = *head;
    while (tmp->next) tmp = tmp->next;
    tmp->next = frag;
}

t_fragment *parse_fragments(const char *text) {
    t_fragment *fragments = NULL;
    int i = 0;
    while (text[i]) 
    {
        t_quote_type qtype = QUOTE_NONE;
        int start = i;
        bool space_after = false;

        // Saltar espacios y ver si hay espacio antes
        while (text[i] && ft_isspace((unsigned char)text[i])) i++;
        if (!text[i]) break;

        start = i;
        if (text[i] == '\'') { // simple quote
            qtype = QUOTE_SINGLE;
            i++;
            start = i;
            while (text[i] && text[i] != '\'') i++;
            space_after = text[i] && ft_isspace((unsigned char)text[i + 1]);
            append_fragment(&fragments, new_fragment(&text[start], i - start, qtype, space_after));
            if (text[i] == '\'') i++;
        } else if (text[i] == '"') { // double quote
            qtype = QUOTE_DOUBLE;
            i++;
            start = i;
            while (text[i] && text[i] != '"') {
                if (text[i] == '\\' && text[i + 1]) i += 2;
                else i++;
            }
            space_after = text[i] && ft_isspace((unsigned char)text[i + 1]);
            append_fragment(&fragments, new_fragment(&text[start], i - start, qtype, space_after));
            if (text[i] == '"') i++;
        } else if (text[i] == '|' || text[i] == '<' || text[i] == '>') { // operators
            start = i;
            if (text[i] == '<' && text[i + 1] == '<') {
                i += 2; // heredoc <<
            } else if (text[i] == '>' && text[i + 1] == '>') {
                i += 2; // append >>
            } else {
                i++; // single character operator
            }
            space_after = text[i] && ft_isspace((unsigned char)text[i]);
            append_fragment(&fragments, new_fragment(&text[start], i - start, QUOTE_NONE, space_after));
        } else { // texto normal
            while (text[i] && !ft_isspace((unsigned char)text[i]) && text[i] != '\'' && text[i] != '"' 
                   && text[i] != '|' && text[i] != '<' && text[i] != '>') i++;
            space_after = text[i] && ft_isspace((unsigned char)text[i]);
            append_fragment(&fragments, new_fragment(&text[start], i - start, QUOTE_NONE, space_after));
        }
    }
    //print_fragments(fragments);
    return fragments;
}

void	print_fragments(t_fragment *fragments)
{
	int	i;

	i = 0;
	while (fragments)
	{
		printf("Fragment %d:\n", i);
		printf("  text           : \"%s\"\n", fragments->text);
		printf("  quote_type     : %d\n", fragments->quote_type);
		printf("  has_space_after: %s\n",
			fragments->has_space_after ? "true" : "false");
		printf("  next           : %p\n\n", (void *)fragments->next);
		fragments = fragments->next;
		i++;
	}
}
