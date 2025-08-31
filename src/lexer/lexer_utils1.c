/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 14:09:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 19:14:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	skip_spaces(const char *input, int i)
{
	while (input[i] != '\0' && ft_isspace((unsigned char)input[i]))
		i++;
	return (i);
}

int	detect_token_type(const char *input, int i)
{
	if (input[i] == '|')
		return (TOKEN_PIPE);
	if (input[i] == '<')
	{
		if (input[i + 1] == '<')
			return (TOKEN_HEREDOC);
		return (TOKEN_REDIRECT_IN);
	}
	if (input[i] == '>')
	{
		if (input[i + 1] == '>')
			return (TOKEN_APPEND);
		return (TOKEN_REDIRECT_OUT);
	}
	return (TOKEN_WORD);
}

char	*extract_token_text(const char *input, int *i, int type)
{
	int start = *i;
	int len = 0;

	if (type == TOKEN_PIPE || type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT)
		len = 1;
	else if (type == TOKEN_HEREDOC || type == TOKEN_APPEND)
		len = 2;
	else if (type == TOKEN_WORD)
	{
		while (input[*i] != '\0' && !ft_isspace((unsigned char)input[*i])
			&& detect_token_type(input, *i) == TOKEN_WORD)
		{
			(*i)++;
			len++;
		}
	}
	if (type == TOKEN_HEREDOC || type == TOKEN_APPEND || type == TOKEN_PIPE
		|| type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT)
		*i += len;
	char *res = malloc(len + 1);
	if (!res)
		return NULL;
	ft_strlcpy(res, &input[start], len + 1);
	return res;
}

/* 
** Construye un nuevo token con texto + tipo + lista de fragmentos.
*/
t_token *create_token(t_token_type type, bool space_before) 
{
    t_token *tok = malloc(sizeof(t_token));
    tok->type = type;
    tok->fragments = NULL;
    tok->has_space_before = space_before;
    tok->next = NULL;
    return tok;
}

t_token *create_token_from_fragments(t_fragment *first_frag, bool space_before)
{
    t_token *tok = malloc(sizeof(t_token));
    if (!tok) return NULL;

    tok->type = TOKEN_WORD;
    tok->fragments = first_frag;
    tok->has_space_before = space_before;
    tok->next = NULL;

    return tok;
}

/* 
** Añade un token al final de la lista.
*/
void append_token(t_token **head, t_token *tok) 
{
    if (!tok) return;
    if (!*head) { *head = tok; return; }
    t_token *tmp = *head;
    while (tmp->next) tmp = tmp->next;
    tmp->next = tok;
}

