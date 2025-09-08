/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/08 18:45:48 by ekakhmad         ###   ########.fr       */
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
        if (frag->expanded_text) // Check if expanded_text is NULL before freeing
            free(frag->expanded_text);
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
        free(tokens->final_text);  // Free the final_text field to prevent memory leak
        free(tokens);
        tokens = tmp;
    }
}
   

t_token_type determine_token_type(char *str)
{
    if (!str)
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