/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_word_validation.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 19:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/30 14:57:39 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

/* 
 * Check if a word token has redirection characters in it
 * like "echo>" or "cat<" which should be treated as syntax errors
 */
int validate_word_token(const char *str)
{
    int len;
    
    if (!str)
        return (1);
    
    len = ft_strlen(str);
    if (len == 0)
        return (1);
    
    /* Check if the word ends with a redirection character */
    if (str[len - 1] == '>' || str[len - 1] == '<' || str[len - 1] == '|')
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", str[len - 1]);
        g_tokenizer_syntax_error = 1;
        return (0);
    }
    
    /* Check if the word has redirection characters within it */
    for (int i = 0; i < len - 1; i++)
    {
        if ((str[i] == '>' || str[i] == '<') && str[i+1] != '>' && str[i+1] != '<')
        {
            fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", str[i]);
            g_tokenizer_syntax_error = 1;
            return (0);
        }
        
        /* Check for pipe characters within words */
        if (str[i] == '|')
        {
            fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", str[i]);
            g_tokenizer_syntax_error = 1;
            return (0);
        }
    }
    
    return (1);
}
