/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 21:10:57 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 00:51:32 by djuarez          ###   ########.fr       */
/*   Created: 2025/08/31 03:34:47 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/08/31 03:34:47 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token *tokenize_input(const char *input)
{
    if (check_unmatched_quotes(input))
        return NULL;

    t_fragment *frags = parse_mixed_fragments(input);
    if (!frags)
        return NULL;

    t_token *tokens = NULL;
    t_token *last_token = NULL;
    bool space_before = false;

    t_fragment *cur = frags;
    while (cur)
    {
        while (cur && strlen(cur->text) == 0 && cur->quote_type == QUOTE_NONE)
            cur = cur->next;
        if (!cur)
            break;

        /* If the fragment is inside quotes, it must be treated as a WORD
         * even if its text looks like an operator (e.g. '|' inside quotes). */
        t_token_type op_type;
        if (cur->quote_type == QUOTE_NONE)
            op_type = determine_token_type(cur->text);
        else
            op_type = TOKEN_WORD;

        if (op_type != TOKEN_WORD)
        {
            t_token *tok = create_token(op_type, space_before);
            space_before = cur->has_space_after;

            t_fragment *frag_copy = new_fragment(cur->text, strlen(cur->text),
                                                 cur->quote_type, cur->has_space_after);
            append_fragment(&tok->fragments, frag_copy);

            tok->final_text = concat_fragments(tok->fragments);

            if (!tokens)
                tokens = tok;
            else
                last_token->next = tok;
            last_token = tok;

            cur = cur->next;
            continue;
        }

        t_token *tok = create_token(TOKEN_WORD, space_before);
        space_before = false;

            while (cur)
        {
            t_fragment *frag_copy = new_fragment(cur->text, strlen(cur->text),
                                                 cur->quote_type, cur->has_space_after);
            append_fragment(&tok->fragments, frag_copy);

            if (cur->has_space_after)
            {
                space_before = true;
                cur = cur->next;
                break;
            }
            cur = cur->next;

            /* Stop if the next fragment is an unquoted operator. Quoted
             * fragments that look like operators should not break the word. */
            if (cur && (cur->quote_type == QUOTE_NONE && determine_token_type(cur->text) != TOKEN_WORD))
                break;
        }

        /* The token type is an operator only when the first fragment is
         * unquoted and its text matches an operator; otherwise it's a WORD. */
        if (tok->fragments->quote_type == QUOTE_NONE)
            tok->type = determine_token_type(tok->fragments->text);
        else
            tok->type = TOKEN_WORD;
        tok->final_text = concat_fragments(tok->fragments);

        if (!tokens)
            tokens = tok;
        else
            last_token->next = tok;
        last_token = tok;
    }

    free_fragments(frags);
    tokens = append_token_eof(tokens);
    return tokens;
}
 