/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 20:45:00 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/09/27 20:45:00 by ekakhmad        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parse_loop_helper(t_parse_ctx *ctx)
{
	t_cmd	*new_cmd;

	while (ctx->cur && ctx->cur->type != TOKEN_EOF)
	{
		if (check_invalid_pipe(ctx->cur))
			return (1);
		new_cmd = create_cmd_node(&ctx->cur, ctx->envp, ctx->state);
		if (!new_cmd)
		{
			free_cmds(ctx->head);
			return (1);
		}
		add_cmd_node(&ctx->head, &ctx->last, new_cmd);
	}
	return (0);
}

int	check_trailing_pipe(t_parse_ctx *ctx)
{
	if (ctx->last && ctx->last->pipe)
	{
		print_error(NULL, "syntax error: unexpected end of file");
		if (ctx->state)
			ctx->state->last_status = 2;
		free_cmds(ctx->head);
		return (1);
	}
	return (0);
}
