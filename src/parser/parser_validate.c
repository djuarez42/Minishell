/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_validate.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 19:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/13 17:20:14 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

/* Check if token is a redirection token */
static int is_redirection_token(t_token_type type)
{
    return (type == TOKEN_REDIRECT_IN || 
            type == TOKEN_REDIRECT_OUT || 
            type == TOKEN_APPEND || 
            type == TOKEN_HEREDOC);
}

/* Get printable representation of token type for error messages */
static const char *get_token_symbol(t_token_type type)
{
    if (type == TOKEN_REDIRECT_IN)
        return "<";
    else if (type == TOKEN_REDIRECT_OUT)
        return ">";
    else if (type == TOKEN_APPEND)
        return ">>";
    else if (type == TOKEN_HEREDOC)
        return "<<";
    else if (type == TOKEN_PIPE)
        return "|";
    else if (type == TOKEN_EOF)
        return "newline";
    else
        return "unknown";
}

/* Validate token syntax - returns 1 if valid, 0 if invalid */
int validate_token_syntax(t_token *tokens)
{
    t_token *cur = tokens;
    
    /* Check for commands starting with a pipe */
    if (cur && cur->type == TOKEN_PIPE)
    {
        fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
        return (0);
    }
    
    while (cur && cur->type != TOKEN_EOF)
    {
        /* Check for redirection without a valid filename */
        if (is_redirection_token(cur->type))
        {
            if (cur->next == NULL || cur->next->type == TOKEN_EOF || 
                is_redirection_token(cur->next->type) || 
                cur->next->type == TOKEN_PIPE)
            {
                fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n", 
                        cur->next ? get_token_symbol(cur->next->type) : "newline");
                return (0);
            }
        }
        
        /* Check for pipe errors */
        if (cur->type == TOKEN_PIPE)
        {            
            /* Pipe at end or followed by another pipe */
            if (cur->next == NULL || cur->next->type == TOKEN_EOF || 
                cur->next->type == TOKEN_PIPE)
            {
                fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n", 
                        cur->next ? "|" : "newline");
                return (0);
            }
        }
        
        cur = cur->next;
    }
    
    return (1);
}

/* Check if a single token is a valid redirection sequence */
int validate_redirection_token(const char *str)
{
    int i;
    char first_char;
    
    i = 0;
    first_char = str[0];
    
    /* Check for valid redirection token: >, <, >>, << */
    if (str[0] == '>' && str[1] == '>' && str[2] == '\0')
        return (1);
    else if (str[0] == '<' && str[1] == '<' && str[2] == '\0')
        return (1);
    else if (str[0] == '>' && str[1] == '\0')
        return (1);
    else if (str[0] == '<' && str[1] == '\0')
        return (1);
    
    /* Check for invalid redirection sequences */
    if (str[0] == '>' || str[0] == '<')
    {
        /* Check for mixed redirection characters like >< */
        while (str[i] != '\0')
        {
            if (str[i] != '\0' && str[i] != first_char && (str[i] == '>' || str[i] == '<'))
            {
                fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", str[i]);
                g_exit_code = 258;
                return (0);
            }
            i++;
        }
        
        /* Reset i for the next check */
        i = 0;
        
        /* Check for too many of the same redirection character */
        while (str[i] == first_char)
            i++;
            
        if (i > 2)
        {
            fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", first_char);
            g_exit_code = 258;
            return (0);
        }
        
        /* Check for incomplete double redirections like '><' */
        if (i == 2 && str[0] != str[1])
        {
            fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", str[1]);
            g_exit_code = 258;
            return (0);
        }
    }
    
    return (1);
}
