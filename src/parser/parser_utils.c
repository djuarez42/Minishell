/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 21:52:11 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_proc_ctx(t_proc_ctx *ctx, t_cmd *cmd, char **envp,
		t_exec_state *state)
{
	static int	argc_argv;
	static int	argc_final_text;

	argc_argv = 0;
	argc_final_text = 0;
	ctx->cmd = cmd;
	ctx->argc_argv = &argc_argv;
	ctx->argc_final_text = &argc_final_text;
	ctx->envp = envp;
	ctx->state = state;
}

static t_token	*process_word_tokens(t_token *cur, t_proc_ctx *ctx)
{
	while (cur && cur->type == TOKEN_WORD)
	{
		ctx->cmd->argv = process_token_with_quotes(cur, ctx);
		if (!ctx->cmd->argv)
		{
			return (NULL);
		}
		cur = cur->next;
	}
	return (cur);
}

t_token	*parse_arguments(t_token *cur, t_cmd *cmd, char **envp,
		t_exec_state *state)
{
	t_proc_ctx	ctx;

	init_proc_ctx(&ctx, cmd, envp, state);
	cur = process_word_tokens(cur, &ctx);
	if (!cur)
		return (NULL);
	cmd->argv[*ctx.argc_argv] = NULL;
	cmd->argv_quote[*ctx.argc_argv] = QUOTE_NONE;
	if (cmd->argv_final_text)
		cmd->argv_final_text[*ctx.argc_final_text] = NULL;
	return (cur);
}

t_token	*parse_arguments_ctx(t_token *cur, t_proc_ctx *ctx)
{
	cur = process_word_tokens(cur, ctx);
	if (!cur)
		return (NULL);
	ctx->cmd->argv[*ctx->argc_argv] = NULL;
	ctx->cmd->argv_quote[*ctx->argc_argv] = QUOTE_NONE;
	if (ctx->cmd->argv_final_text)
		ctx->cmd->argv_final_text[*ctx->argc_final_text] = NULL;
	return (cur);
}

t_token	*parse_redirections(t_token *cur, t_cmd *cmd)
{
	t_redir	*new_redir;
	t_redir	*last;

	if (!cur)
		return (NULL);
	if (!validate_redirection(cur))
		return (NULL);
	new_redir = create_redir(cur);
	if (!new_redir)
		return (NULL);
	if (!cmd->redirs)
		cmd->redirs = new_redir;
	else
	{
		last = cmd->redirs;
		while (last->next)
			last = last->next;
		last->next = new_redir;
	}
	return (cur->next->next);
}
