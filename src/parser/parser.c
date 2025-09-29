/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/29 19:32:46 by djuarez          ###   ########.fr       */
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

static int	validate_pipe_syntax(t_token *cur, t_token_type prev_type,
	t_exec_state *state)
{
	if (cur->type != TOKEN_PIPE)
		return (0);
	if (prev_type == TOKEN_NONE || prev_type == TOKEN_PIPE)
	{
		print_error(NULL, "syntax error near unexpected token `|'");
		if (state)
			state->last_status = 2;
		return (1);
	}
	if (!cur->next || cur->next->type == TOKEN_PIPE)
	{
		print_error(NULL, "syntax error near unexpected token `|'");
		if (state)
			state->last_status = 2;
		return (1);
	}

	return (0);
}

int	parse_loop_helper(t_parse_ctx *ctx)
{
	t_cmd			*new_cmd;
	t_token_type	prev_type;

	prev_type = TOKEN_NONE;
	while (ctx->cur && ctx->cur->type != TOKEN_EOF)
	{
		if (validate_pipe_syntax(ctx->cur, prev_type, ctx->state))
			return (1);
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
