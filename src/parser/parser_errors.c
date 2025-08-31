/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_errors.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 23:20:20 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/30 23:20:20 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "parser.h"

/* 
 * Validates pipe syntax in the token stream:
 * - Cannot have a pipe at the beginning
 * - Cannot have consecutive pipes
 * - Cannot have a pipe at the end
 */
int	validate_pipe_syntax(t_token *tokens)
{
	t_token *cur;
	
	if (!tokens)
		return (1);
	
	/* Check if first token is a pipe */
	if (tokens->type == TOKEN_PIPE)
	{
		ft_putendl_fd("minishell: syntax error near unexpected token `|'", STDERR_FILENO);
		return (0);
	}
	
	cur = tokens;
	while (cur && cur->next && cur->next->type != TOKEN_EOF)
	{
		/* Check for consecutive pipes */
		if (cur->type == TOKEN_PIPE && cur->next->type == TOKEN_PIPE)
		{
			ft_putendl_fd("minishell: syntax error near unexpected token `|'", STDERR_FILENO);
			return (0);
		}
		cur = cur->next;
	}
	
	/* Check if last token before EOF is a pipe */
	if (cur && cur->type == TOKEN_PIPE)
	{
		ft_putendl_fd("minishell: syntax error near unexpected token `|'", STDERR_FILENO);
		return (0);
	}
	
	return (1);
}
