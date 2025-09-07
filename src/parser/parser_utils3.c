/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 02:39:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/07 22:21:01 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *concat_final_text(t_token *tok)
{
    t_fragment *frag = tok->fragments;
    char *final_text = ft_strdup("");
    char *tmp;
    char *new_result;

    while (frag)
    {
        if (frag->expanded_text)
            tmp = ft_strdup(frag->expanded_text);
        else
            tmp = ft_strdup(frag->text);

        new_result = str_append(final_text, tmp);
        free(tmp);
        final_text = new_result;

        frag = frag->next;
    }
    return (final_text);
}

int should_expand_fragment(t_fragment *frag)
{
    int i;

    if (!frag || !frag->text)
        return 0;

    /* Nunca expandir dentro de comillas simples */
    if (frag->quote_type == QUOTE_SINGLE)
        return 0;

    i = 0;
    while (frag->text[i])
    {
        if (frag->text[i] == '$')
            return 1; /* expandir variables */
        if (frag->text[i] == '~' && i == 0)
            return 1; /* tilde al inicio */
        i++;
    }
    return 0; /* nada que expandir */
}

char **build_argv_from_fragments(t_token *tok, t_proc_ctx *ctx)
{
    t_fragment *frag = tok->fragments;
    char *tmp;

    while (frag)
    {
        if (frag->expanded_text && frag->expanded_text[0] != '\0')
        {
            tmp = ft_strdup(frag->expanded_text);
            if (tmp)
            {
                ctx->cmd->argv[*ctx->argc_argv] = tmp;
                ctx->cmd->argv_quote[*ctx->argc_argv] = frag->quote_type;
                (*ctx->argc_argv)++;
            }
        }
        frag = frag->next;
    }
    return ctx->cmd->argv;
}