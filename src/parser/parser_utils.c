/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 17:12:43 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Free argv entries up to argc (if argc >= 0), and argv/argv_quote arrays.
   Does not free cmd itself. */
void	free_partial_cmd(t_cmd *cmd, int argc)
{
	int i;

	if (!cmd)
		return;
	if (cmd->argv)
	{
		if (argc >= 0)
		{
			i = 0;
			while (i < argc)
			{
				free(cmd->argv[i]);
				i++;
			}
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	if (cmd->argv_quote)
	{
		free(cmd->argv_quote);
		cmd->argv_quote = NULL;
	}
}

void free_cmds(t_cmd *cmd)
{
    t_cmd *tmp;
    int i;

    while (cmd)
    {
        tmp = cmd->next;

        if (cmd->argv)
        {
            i = 0;
            while (cmd->argv[i])
                free(cmd->argv[i++]);
            free(cmd->argv);
        }

        if (cmd->argv_final_text)
        {
            i = 0;
            while (cmd->argv_final_text[i])
                free(cmd->argv_final_text[i++]);
            free(cmd->argv_final_text);
        }

        if (cmd->argv_quote)
            free(cmd->argv_quote);

        if (cmd->argv_first_word)
            free(cmd->argv_first_word);

        if (cmd->redirs)
            free_redirs(cmd->redirs);

        free(cmd);
        cmd = tmp;
    }
}

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;
	int		i;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		if (redir->heredoc_content)
		{
			i = 0;
			while (redir->heredoc_content[i])
				free(redir->heredoc_content[i++]);
			free(redir->heredoc_content);
		}
		free(redir);
		redir = tmp;
	}
}

void free_str_array(char **arr)
{
    int i = 0;
    if (!arr)
        return;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

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

    // 1) calcular longitud total
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

    //2) rellenar S y splittable mask 
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

    // 3) contar palabras
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

    // NUEVO: si count == 0 pero hay fragments quoted, generar palabra vacía
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
        // si no hay fragments quoted, seguimos devolviendo NULL
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

    // 4) extraer substrings
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

char **process_token_with_quotes(t_token *tok, t_proc_ctx *ctx)
{
    t_fragment *frag;
    int is_assignment = 0;
    int idx;

    if (!tok || !ctx || !ctx->cmd)
        return NULL;

    idx = *ctx->argc_argv;

    // 1) expandir fragments
    expand_fragments(tok, ctx->envp, ctx->state);

    // 2) construir final_text 
    free(tok->final_text);
    tok->final_text = concat_final_text(tok);

    // 3) guardar final_text
    if (ctx->cmd->argv_final_text)
    {
        ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup(tok->final_text ? tok->final_text : "");
        (*ctx->argc_final_text)++;
    }

    // 4) detectar assignment
    frag = tok->fragments;
    if (frag && ft_strchr(frag->text ? frag->text : "", '='))
        is_assignment = 1;

    // 5) construir argv
    if (is_assignment)
    {
        ctx->cmd->argv[idx] = ft_strdup(tok->final_text ? tok->final_text : "");
        ctx->cmd->argv_quote[idx] = detect_combined_quote(tok->fragments);
        idx++;
    }
    else
    {
        int nwords = 0;
        char **words = build_words_from_token(tok, &nwords);

        if (words)
        {
            for (int w = 0; w < nwords; w++)
            {
                ctx->cmd->argv[idx] = ft_strdup(words[w]);
                ctx->cmd->argv_quote[idx] = detect_combined_quote(tok->fragments);
                idx++;
            }
            free_str_array(words);
        }
    }

    *ctx->argc_argv = idx;
    return ctx->cmd->argv;
}



char **ft_split_spaces(const char *s)
{
    int count = 0;
    int in_word = 0;
    const char *p = s;
    char **res;
    int w;
    int len;

    if (!s)
        return NULL;

    while (*p)
    {
        if (!isspace((unsigned char)*p))
        {
            if (!in_word)
            {
                count++;
                in_word = 1;
            }
        }
        else
            in_word = 0;
        p++;
    }

    res = malloc(sizeof(char *) * (count + 1));
    if (!res)
        return NULL;

    w = 0;
    while (*s)
    {
        while (*s && isspace((unsigned char)*s))
            s++;
        if (!*s)
            break;

        len = 0;
        while (s[len] && !isspace((unsigned char)s[len]))
            len++;

        res[w++] = ft_strndup(s, len);
        s += len;
    }
    res[w] = NULL;
    return res;
}

t_quote_type detect_combined_quote(t_fragment *frags)
{
    t_quote_type result = QUOTE_NONE;
    t_fragment *cur = frags;

    while (cur)
    {
        if (cur->quote_type == QUOTE_SINGLE)
        {
            if (result == QUOTE_NONE)
                result = QUOTE_SINGLE;
            else if (result != QUOTE_SINGLE)
                return QUOTE_MIXED;
        }
        else if (cur->quote_type == QUOTE_DOUBLE)
        {
            if (result == QUOTE_NONE)
                result = QUOTE_DOUBLE;
            else if (result != QUOTE_DOUBLE)
                return QUOTE_MIXED;
        }
        else if (cur->quote_type == QUOTE_DOLLAR)
        {
            return QUOTE_DOLLAR;
        }
        else if (cur->quote_type == QUOTE_MIXED)
        {
            return QUOTE_MIXED;
        }
        // QUOTE_NONE → no cambia el resultado
        cur = cur->next;
    }
    return result;
}
