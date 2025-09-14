/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 01:01:49 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token_type lx_meta_type(const char *s, int *consumed)
{
    if (s[0] == '<' && s[1] == '<') {
        *consumed = 2;
        return TOKEN_HEREDOC;  // Detectamos '<<'
    }
    if (s[0] == '>' && s[1] == '>') {
        *consumed = 2;
        return TOKEN_APPEND;  // Detectamos '>>'
    }
    if (s[0] == '<') {
        *consumed = 1;
        return TOKEN_REDIRECT_IN;  // Detectamos '<'
    }
    if (s[0] == '>') {
        *consumed = 1;
        return TOKEN_REDIRECT_OUT;  // Detectamos '>'
    }
    if (s[0] == '|') {
        *consumed = 1;
        return TOKEN_PIPE;  // Detectamos '|'
    }
    return -1; // No es un operador
}

int check_unmatched_quotes(const char *input)
{
    int single_open = 0;
    int double_open = 0;
    int i = 0;

    while (input[i])
    {
        if (input[i] == '\'' && double_open == 0) // solo cuenta si NO estás dentro de ""
            single_open = !single_open;
        else if (input[i] == '"' && single_open == 0) // solo cuenta si NO estás dentro de ''
            double_open = !double_open;
        i++;
    }
    if (single_open || double_open)
    {
        fprintf(stderr, "minishell: syntax error: unmatched quotes\n");
        return (1); // error
    }
    return (0); // ok
}


t_token *create_token_group(t_fragment *frag_head, t_token_type type)
{
    t_token *tok = malloc(sizeof(t_token));
    if (!tok)
        return NULL;
    tok->type = type;
    tok->fragments = frag_head;
    tok->next = NULL;
    return tok;
}
