/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 00:58:43 by djuarez          ###   ########.fr       */
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

void assign_token_types(t_token *tokens)
{
    while (tokens)
    {
        char *text = tokens->fragments->text;
        
        if (strcmp(text, "|") == 0)
            tokens->type = TOKEN_PIPE;
        else if (strcmp(text, "<") == 0)
            tokens->type = TOKEN_REDIRECT_IN;
        else if (strcmp(text, ">") == 0)
            tokens->type = TOKEN_REDIRECT_OUT;
        else if (strcmp(text, "<<") == 0)
            tokens->type = TOKEN_HEREDOC;
        else if (strcmp(text, ">>") == 0)
            tokens->type = TOKEN_APPEND;
        else if (text[0] == '|' && text[1] != '\0')
        {
            // Token starts with pipe but has more content - syntax error will be caught in parser
            tokens->type = TOKEN_PIPE;
        }
        else if (text[0] == '<' && text[1] != '\0' && !(text[1] == '<' && text[2] == '\0'))
        {
            // Token starts with < but isn't just "<" or "<<"
            tokens->type = TOKEN_REDIRECT_IN;
        }
        else if (text[0] == '>' && text[1] != '\0' && !(text[1] == '>' && text[2] == '\0'))
        {
            // Token starts with > but isn't just ">" or ">>"
            tokens->type = TOKEN_REDIRECT_OUT;
        }
        else
            tokens->type = TOKEN_WORD;

        tokens = tokens->next;
    }
}
