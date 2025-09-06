/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 02:39:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/06 20:06:11 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


char *build_final_text(t_token *tok, char **envp, t_exec_state *state)
{
    t_fragment *f;
    char *result;
    char *tmp;

    if (!tok || !tok->fragments)
        return ft_strdup("");

    result = ft_strdup("");
    if (!result)
        return NULL;

    f = tok->fragments;
    while (f)
    {
        if (should_expand_fragment(f))
            tmp = expand_variables(f->text, envp, state, f->quote_type);
        else
            tmp = ft_strdup(f->text);

        if (!tmp)
        {
            free(result);
            return NULL;
        }

        char *new_res = str_append(result, tmp);
        free(tmp);
        free(result);
        result = new_res;

        f = f->next;
    }

    return result;
}


int should_expand_fragment(t_fragment *frag)
{
    int i;

    if (!frag || !frag->text)
        return (0);

    // No expandir nunca dentro de comillas simples
    if (frag->quote_type == QUOTE_SINGLE)
        return (0);

    i = 0;
    while (frag->text[i])
    {
        if (frag->text[i] == '$')
            return (1); // hay un dólar y se permite expandir
        if (frag->text[i] == '~' && i == 0)
            return (1); // tilde al inicio
        i++;
    }
    return (0); // no hay nada a expandir
}