/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 03:34:47 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/08/31 03:34:47 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token *tokenize_input(const char *input)
{
    t_fragment *frags;
    t_fragment *cur_frag;
    t_fragment *last_frag;
    t_token *tokens = NULL;
    t_token *tok;
    int type;
    int consumed;

    printf("=== DEBUG tokenize_input ===\n");

    // Paso 1: Obtener los fragmentos de la entrada (tokens parciales)
    frags = parse_fragments(input);
    cur_frag = frags;

    // Paso 2: Iterar sobre los fragmentos para crear los tokens
    while (cur_frag)
    {
        // Saltar fragmentos vacíos
        while (cur_frag && strlen(cur_frag->text) == 0)
            cur_frag = cur_frag->next;

        if (!cur_frag)
            break;

        type = -1;
        consumed = 0;

        // Detectar operadores y redirecciones solo si no está entre comillas
        if (cur_frag->quote_type == QUOTE_NONE)
            type = lx_meta_type(cur_frag->text, &consumed);

        if (type != -1) // Es operador o redirección
        {
            tok = create_token(NULL, type);
            tok->fragments = cur_frag;

            last_frag = cur_frag;
            cur_frag = cur_frag->next;
            last_frag->next = NULL;

            append_token(&tokens, tok);
            printf(" → Creando token tipo %d (operador o redirección)\n", type);
            continue;
        }

        // Caso normal: palabra (puede tener varios fragmentos)
        tok = create_token(NULL, TOKEN_WORD);

        // Asignar el primer fragmento no vacío
        tok->fragments = cur_frag;
        last_frag = cur_frag;
        cur_frag = cur_frag->next;
        last_frag->next = NULL;

        // Concatenar fragmentos contiguos (pueden tener comillas)
        while (cur_frag)
        {
            if (strlen(cur_frag->text) == 0) // saltar fragmentos vacíos
            {
                cur_frag = cur_frag->next;
                continue;
            }

            // Fragmento válido → enlazar al token
            last_frag->next = cur_frag;
            last_frag = cur_frag;
            cur_frag = cur_frag->next;
            last_frag->next = NULL;
        }

        append_token(&tokens, tok);
        printf(" → Creando token tipo %d (palabra)\n", TOKEN_WORD);
    }

    return tokens;
}
