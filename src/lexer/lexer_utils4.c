/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 23:40:33 by djuarez          ###   ########.fr       */
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
    int single_quote = 0;
    int double_quote = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '\'')
            single_quote = !single_quote;
        else if (input[i] == '"')
            double_quote = !double_quote;
    }

    if (single_quote || double_quote)
    {
        fprintf(stderr, "Syntax error: unmatched quote\n");
        return 1; // Error si las comillas no están balanceadas
    }
    return 0; // Todo está bien
}

void print_tokens(t_token *tokens)
{
    t_token *cur = tokens;
    int i = 0;

    // Comment out debug print in production
    while (cur)
    {
        cur = cur->next;
        i++;
    }
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
        if (strcmp(tokens->fragments->text, "|") == 0)
            tokens->type = TOKEN_PIPE;
        else if (strcmp(tokens->fragments->text, "<") == 0)
            tokens->type = TOKEN_REDIRECT_IN;
        else if (strcmp(tokens->fragments->text, ">") == 0)
            tokens->type = TOKEN_REDIRECT_OUT;
        else if (strcmp(tokens->fragments->text, "<<") == 0)
            tokens->type = TOKEN_HEREDOC;
        else if (strcmp(tokens->fragments->text, ">>") == 0)
            tokens->type = TOKEN_APPEND;
        else
            tokens->type = TOKEN_WORD;

        tokens = tokens->next;
    }
}
