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
