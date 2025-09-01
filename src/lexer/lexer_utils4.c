/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 21:18:35 by ekakhmad         ###   ########.fr       */
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

/* DEBUG FUNCTION - COMMENTED OUT
void print_tokens(t_token *tokens)
{
    t_token *cur = tokens;
    int i = 0;

    printf("=== FINAL TOKENS ===\n");
    while (cur)
    {
        printf("Token %d @%p: type=%d, has_space_before=%d\n",
               i, (void*)cur, cur->type, cur->has_space_before);

        if (cur->final_text)
            printf("  Final text: \"%s\"\n", cur->final_text);
        else
            printf("  Final text: (NULL)\n");

        t_fragment *frag = cur->fragments;
        int j = 0;
        while (frag)
        {
            printf("  Frag %d @%p: \"%s\" (quote=%d, space_after=%d, next=%p)\n",
                   j, (void*)frag, frag->text,
                   frag->quote_type, frag->has_space_after, (void*)frag->next);
            frag = frag->next;
            j++;
        }

        cur = cur->next;
        i++;
    }
    printf("=======================================\n\n");
}
*/

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
