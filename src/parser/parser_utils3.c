/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 02:39:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 20:11:10 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char **build_words_from_token(t_token *tok, int *out_count)
{
    t_fragment *frag;
    size_t total_len = 0;
    size_t pos = 0;
    char *S;
    unsigned char *splittable; 
    char **words = NULL;
    int count = 0;
    size_t i;

    if (!tok || !out_count)
        return NULL;
    frag = tok->fragments;
    while (frag)
    {
        if (frag->expanded_text)
            total_len += ft_strlen(frag->expanded_text);
        frag = frag->next;
    }
    S = malloc(total_len + 1);
    splittable = malloc(total_len);
    if (!S || !splittable)
    {
        free(S);
        free(splittable);
        *out_count = 0;
        return NULL;
    }
    frag = tok->fragments;
    pos = 0;
    while (frag)
    {
        if (frag->expanded_text)
        {
            size_t flen = ft_strlen(frag->expanded_text);
            int frag_splittable = 0;
            if (frag->quote_type == QUOTE_NONE && frag->text && frag->text[0] == '$')
                frag_splittable = 1;

            for (i = 0; i < flen; i++)
            {
                S[pos] = frag->expanded_text[i];
                splittable[pos] = frag_splittable ? 1 : 0;
                pos++;
            }
        }
        frag = frag->next;
    }
    S[pos] = '\0';
    i = 0;
    while (i < pos)
    {
        while (i < pos && ft_isspace((unsigned char)S[i]) && splittable[i])
            i++;
        if (i >= pos)
            break;
        count++;
        while (i < pos && !(ft_isspace((unsigned char)S[i]) && splittable[i]))
            i++;
    }
    if (count == 0)
    {
        int frag_count = 0;
        frag = tok->fragments;
        while (frag)
        {
            if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
                frag_count++;
            frag = frag->next;
        }
        if (frag_count > 0)
        {
            words = malloc(sizeof(char *) * (frag_count + 1));
            if (!words)
            {
                free(S);
                free(splittable);
                *out_count = 0;
                return NULL;
            }
            frag = tok->fragments;
            int wi = 0;
            while (frag)
            {
                if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
                {
                    words[wi] = ft_strdup("");
                    wi++;
                }
                frag = frag->next;
            }
            words[frag_count] = NULL;
            *out_count = frag_count;
            free(S);
            free(splittable);
            return words;
        }
        free(S);
        free(splittable);
        *out_count = 0;
        return NULL;
    }
    words = malloc(sizeof(char *) * (count + 1));
    if (!words)
    {
        free(S);
        free(splittable);
        *out_count = 0;
        return NULL;
    }
    i = 0;
    int wi = 0;
    while (i < pos && wi < count)
    {
        while (i < pos && ft_isspace((unsigned char)S[i]) && splittable[i])
            i++;
        if (i >= pos)
            break;
        size_t start = i;
        while (i < pos && !(ft_isspace((unsigned char)S[i]) && splittable[i]))
            i++;
        size_t wlen = i - start;
        words[wi] = ft_strndup(&S[start], wlen);
        wi++;
    }
    words[wi] = NULL;
    free(S);
    free(splittable);
    *out_count = count;
    return words;
}



void    update_final_text(t_token *tok, t_proc_ctx *ctx)
{
	expand_fragments(tok, ctx->envp, ctx->state);
	free(tok->final_text);
	tok->final_text = concat_final_text(tok);
}
