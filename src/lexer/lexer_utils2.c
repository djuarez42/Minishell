/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:45:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/11 18:16:52 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_fragment *new_fragment(const char *start, size_t len, t_quote_type qtype, bool space_after) 
{
    t_fragment *frag = malloc(sizeof(t_fragment));
    if (!frag) return NULL;
    frag->text = malloc(len + 1);
    if (!frag->text) { free(frag); return NULL; }
    memcpy(frag->text, start, len);
    frag->text[len] = '\0';
    frag->quote_type = qtype;
    frag->has_space_after = space_after;
    frag->next = NULL;

    return frag;
}

void append_fragment(t_fragment **head, t_fragment *frag)
{
    if (!frag) return;
    if (!*head) { *head = frag; return; }
    t_fragment *tmp = *head;
    while (tmp->next) tmp = tmp->next;
    tmp->next = frag;
}

t_fragment *parse_mixed_fragments(const char *text)
{
    t_fragment *fragments = NULL;
    int i = 0;

    while (text[i])
    {
        while (text[i] && ft_isspace((unsigned char)text[i]))
            i++;
        if (!text[i])
            break;

        /* Backslashes */
        if (text[i] == '\\')
        {
            t_fragment *bs = handle_backslashes(text, &i);
            if (bs)
            {
                append_fragment(&fragments, bs);
                continue;
            }
        }

        /* $'...' o $"..." */
        if (is_dollar_string(text, i))
        {
            char quote = text[i + 1]; // puede ser ' o "
            i += 2; // saltamos $' o $"
            int start = i;

            while (text[i] && text[i] != quote)
            {
                if (quote == '"' && text[i] == '\\' && text[i + 1])
                    i += 2; // escapamos dentro de $"
                else
                    i++;
            }

            int len = i - start;
            bool space_after = text[i + 1] && ft_isspace((unsigned char)text[i + 1]);

            if (quote == '\'')
                append_fragment(&fragments,
                    new_fragment(&text[start], (size_t)len, QUOTE_DOLLAR, space_after));
            else
                append_fragment(&fragments,
                    new_fragment(&text[start], (size_t)len, QUOTE_DOUBLE, space_after));

            if (text[i] == quote)
                i++; // saltamos el cierre de ' o "
            continue;
        }

        /* Comillas simples */
        if (text[i] == '\'')
        {
            int start = ++i;
            while (text[i] && text[i] != '\'')
                i++;
            int len = i - start;
            bool space_after = text[i + 1] && ft_isspace((unsigned char)text[i + 1]);
            append_fragment(&fragments, new_fragment(&text[start], (size_t)len, QUOTE_SINGLE, space_after));
            if (text[i] == '\'')
                i++;
            continue;
        }

        /* Comillas dobles */
        else if (text[i] == '"')
        {
            int start = ++i;
            while (text[i] && text[i] != '"')
            {
                if (text[i] == '\\' && text[i + 1])
                    i += 2;
                else
                    i++;
            }
            int len = i - start;
            bool space_after = text[i + 1] && ft_isspace((unsigned char)text[i + 1]);
            append_fragment(&fragments, new_fragment(&text[start], (size_t)len, QUOTE_DOUBLE, space_after));
            if (text[i] == '"')
                i++;
            continue;
        }

        /* Operadores | < > (incluye << y >>) */
        else if (text[i] == '|' || text[i] == '<' || text[i] == '>')
        {
            int start = i;
            if (text[i] == '<' && text[i + 1] == '<')
                i += 2;
            else if (text[i] == '>' && text[i + 1] == '>')
                i += 2;
            else
                i++;
            int len = i - start;
            bool space_after = text[i] && ft_isspace((unsigned char)text[i]);
            append_fragment(&fragments, new_fragment(&text[start], (size_t)len, QUOTE_NONE, space_after));
            continue;
        }

        /* Texto normal o combinaciones */
        else
        {
            int start = i;
            while (text[i] && !ft_isspace((unsigned char)text[i]) &&
                   text[i] != '\'' && text[i] != '"' &&
                   text[i] != '|' && text[i] != '<' && text[i] != '>')
                i++;
            int len = i - start;
            bool space_after = text[i] && ft_isspace((unsigned char)text[i]);
            append_fragment(&fragments, new_fragment(&text[start], (size_t)len, QUOTE_NONE, space_after));
            continue;
        }
    }

    //print_fragments(fragments);
    return fragments;
}


void	print_fragments(t_fragment *fragments)
{
	int	i;

	i = 0;
	while (fragments)
	{
		printf("Fragment %d:\n", i);
		printf("  text           : \"%s\"\n", fragments->text);
		printf("  quote_type     : %d\n", fragments->quote_type);
		printf("  has_space_after: %s\n",
			fragments->has_space_after ? "true" : "false");
		printf("  next           : %p\n\n", (void *)fragments->next);
		fragments = fragments->next;
		i++;
	}
}
void print_tokens_raw(t_token *tokens)
{
    int i = 0;
    while (tokens)
    {
        printf("Token %d:\n", i);
        printf("  type           : %d\n", tokens->type); // enum directo
        printf("  has_space_before: %s\n", tokens->has_space_before ? "true" : "false");
        printf("  final_text     : \"%s\"\n", tokens->final_text ? tokens->final_text : "(null)");

        // Imprimir fragments de este token
        t_fragment *frag = tokens->fragments;
        int j = 0;
        while (frag)
        {
            printf("    Fragment %d: text=\"%s\", quote_type=%d, has_space_after=%s\n",
                   j,
                   frag->text,
                   frag->quote_type,      // enum directo
                   frag->has_space_after ? "true" : "false");
            frag = frag->next;
            j++;
        }

        printf("\n");
        tokens = tokens->next;
        i++;
    }
}

t_fragment *handle_backslashes(const char *text, int *i)
{
    int start = *i;
    int count = 0;

    /* contar backslashes consecutivos */
    while (text[*i] && text[*i] == '\\')
    {
        count++;
        (*i)++;
    }

    if (count == 0)
        return NULL;

    char next = text[*i];

    if (next == '$')
    {
        int keep = count / 2; /* la mitad de los backslashes se quedan */

        if (count % 2 == 0)
        {
            /* N par: devolvemos sólo los backslashes conservados */
            if (keep > 0)
            {
                char *buf = malloc((size_t)keep + 1);
                if (!buf)
                    return NULL;
                for (int j = 0; j < keep; j++)
                    buf[j] = '\\';
                buf[keep] = '\0';

                bool space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
                t_fragment *f = new_fragment(buf, keep, QUOTE_NONE, space_after);
                free(buf);
                return f;
            }
            /* keep == 0 → no devolvemos fragmento, dejamos que el parser maneje el '$' */
            return NULL;
        }
        else
        {
            /* N impar: devolvemos backslashes + '$' literal */
            int buflen = keep + 1;
            char *buf = malloc((size_t)buflen + 1);
            if (!buf)
                return NULL;
            for (int j = 0; j < keep; j++)
                buf[j] = '\\';
            buf[keep] = '$';
            buf[buflen] = '\0';

            (*i)++; /* consumir el '$' */

            bool space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
            t_fragment *f = new_fragment(buf, buflen, QUOTE_NONE, space_after);
            free(buf);
            return f;
        }
    }

    /* Caso general: no hay '$' después → todos los backslashes son literales */
    bool space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
    t_fragment *f = new_fragment(&text[start], (size_t)count, QUOTE_NONE, space_after);
    return f;
}