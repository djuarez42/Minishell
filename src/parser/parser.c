/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 21:41:37 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_initial_pipe(t_token *cur)
{
	if (cur && cur->type == TOKEN_PIPE)
	{
		ft_putstr_fd("[PARSER ERR] minishell: syntax error near unexpected token `|'\n", 2);
		return (1);
	}
	return (0);
}

static int	check_invalid_pipe(t_token *cur)
{
	if (cur->type == TOKEN_PIPE && cur->fragments
		&& ft_strlen(cur->fragments->text) > 1)
	{
		ft_putstr_fd("[PARSER ERR] minishell: syntax error near unexpected token `|'\n", 2);
		return (1);
	}
	return (0);
}

static int	parse_loop(t_parse_ctx *ctx)
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

t_cmd	*parser_tokens(t_token *tokens, char **envp, t_exec_state *state)
{
	t_parse_ctx	ctx;
	t_token		*tmp;

	ctx.head = NULL;
	ctx.last = NULL;
	ctx.cur = tokens;
	ctx.envp = envp;
	ctx.state = state;
	if (getenv("MINISHELL_DEBUG_TOKENS") && tokens)
	{
		tmp = tokens;
		while (tmp)
		{
			fprintf(stderr, "[PARSER TOK] type=%d text='%s' has_space_before=%d\n",
				tmp->type, tmp->final_text ? tmp->final_text : "(null)", tmp->has_space_before);
			tmp = tmp->next;
		}
	}
	if (check_initial_pipe(ctx.cur))
	{
		if (ctx.state)
			ctx.state->last_status = 2;
		return (NULL);
	}
	if (parse_loop(&ctx))
	{
		if (ctx.state)
			ctx.state->last_status = 2;
		return (NULL);
	}
	return (ctx.head);
}
