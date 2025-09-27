/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 20:25:23 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_initial_pipe(t_token *cur)
{
	if (cur && cur->type == TOKEN_PIPE)
	{
		print_error(NULL, "syntax error near unexpected token `|'");
		return (1);
	}
	return (0);
}

int	check_invalid_pipe(t_token *cur)
{
	if (cur->type == TOKEN_PIPE && cur->fragments
		&& ft_strlen(cur->fragments->text) > 1)
	{
		print_error(NULL, "syntax error near unexpected token `|'\'");
		return (1);
	}
	return (0);
}

int	parse_loop_helper(t_parse_ctx *ctx);

t_cmd	*parser_tokens(t_token *tokens, char **envp, t_exec_state *state)
{
	t_parse_ctx	ctx;

	ctx.head = NULL;
	ctx.last = NULL;
	ctx.cur = tokens;
	ctx.envp = envp;
	ctx.state = state;
	if (check_initial_pipe(ctx.cur))
	{
		if (ctx.state)
			ctx.state->last_status = 2;
		return (NULL);
	}
	if (parse_loop_helper(&ctx))
	{
		if (ctx.state)
			ctx.state->last_status = 2;
		return (NULL);
	}
	if (check_trailing_pipe(&ctx))
		return (NULL);
	return (ctx.head);
}
