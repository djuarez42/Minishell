/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 21:10:57 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 19:34:39 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token *tokenize_input(const char *input)
{
    t_fragment *frags;
    t_fragment *cur_frag;
    t_token *tokens = NULL;
    t_token *tok;
    int type;

    printf("=== DEBUG tokenize_input ===\n");

    frags = parse_fragments(input);
    cur_frag = frags;

    while (cur_frag)
    {
        // Detectamos tipo de token según primer fragmento
        if (cur_frag->quote_type == QUOTE_NONE)
        {
            // Si es un operador, creamos un token separado
            if (cur_frag->text[0] == '|')
                type = TOKEN_PIPE;
            else if (cur_frag->text[0] == '<')
            {
                if (cur_frag->text[1] == '<')
                    type = TOKEN_HEREDOC;
                else
                    type = TOKEN_REDIRECT_IN;
            }
            else if (cur_frag->text[0] == '>')
            {
                if (cur_frag->text[1] == '>')
                    type = TOKEN_APPEND;
                else
                    type = TOKEN_REDIRECT_OUT;
            }
            else
                type = TOKEN_WORD;  // En todos los demás casos, tratamos como palabra
        }
        else
        {
            // Fragmentos con quotes siempre son WORD
            type = TOKEN_WORD;
        }

        // Creamos un token de tipo determinado
        tok = create_token(NULL, type);

        // Asignamos los fragments al token
        tok->fragments = cur_frag;
        t_fragment *last_frag = cur_frag;
        cur_frag = cur_frag->next;
        last_frag->next = NULL;

        // Si el token es de tipo WORD y hay fragments contiguos con quote_type != NONE
        if (type == TOKEN_WORD)
        {
            while (cur_frag && cur_frag->quote_type != QUOTE_NONE)
            {
                last_frag->next = cur_frag;
                last_frag = cur_frag;
                cur_frag = cur_frag->next;
                last_frag->next = NULL;
            }
        }

        // Debugeo
        printf(" → Creando token tipo %d\n", type);
        append_token(&tokens, tok);
    }

    return tokens;
}

