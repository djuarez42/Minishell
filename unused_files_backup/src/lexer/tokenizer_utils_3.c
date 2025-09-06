/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 01:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/31 01:50:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Function to classify redirection files in the token list
 * Marks tokens that are file names for redirections to handle them specially
 */
void    classify_redirection_files(t_token *token_list)
{
    t_token *current;
    t_token *next;

    if (!token_list)
        return;
    
    current = token_list;
    while (current && current->next)
    {
        next = current->next;
        
        // If current token is a redirection and next is a WORD
        if ((current->type == TOKEN_REDIRECT_IN || 
             current->type == TOKEN_REDIRECT_OUT ||
             current->type == TOKEN_HEREDOC || 
             current->type == TOKEN_APPEND) && 
            next->type == TOKEN_WORD)
        {
            // Mark the next token as a file name for special handling
            // This allows us to handle file names differently in expansion
            if (current->type == TOKEN_REDIRECT_IN || current->type == TOKEN_HEREDOC)
                next->type = TOKEN_IN_FILE;
            else
                next->type = TOKEN_OUT_FILE;
        }
        
        current = current->next;
    }
}
