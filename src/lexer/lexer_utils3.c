/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/14 01:25:01 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_fragments(t_fragment *frag)
{
    t_fragment *tmp;
    while (frag)
    {
        tmp = frag->next;
        free(frag->text);
        free(frag);
        frag = tmp;
    }
}

void	free_token_list(t_token *tokens)
{
	t_token	*tmp;

    while (tokens)
    {
        tmp = tokens->next;
        if (tokens->fragments)
            free_fragments(tokens->fragments);
        free(tokens);
        tokens = tmp;
    }
}
   

t_token_type determine_token_type(char *str, t_quote_type quote)
{
    if (!str)
        return TOKEN_WORD;

    if (quote != QUOTE_NONE) // 🔹 si está entre comillas, siempre WORD
        return TOKEN_WORD;

    if (str[0] == '<' && str[1] == '<' && str[2] == '\0')
        return TOKEN_HEREDOC;
    else if (str[0] == '>' && str[1] == '>' && str[2] == '\0')
        return TOKEN_APPEND;
    else if (str[0] == '<' && str[1] == '\0')
        return TOKEN_REDIRECT_IN;
    else if (str[0] == '>' && str[1] == '\0')
        return TOKEN_REDIRECT_OUT;
    else if (str[0] == '|' && str[1] == '\0')
        return TOKEN_PIPE;
    else
        return TOKEN_WORD;
}
