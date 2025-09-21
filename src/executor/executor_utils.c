/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:34:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 21:42:59 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int	alloc_pids_in_ctx(t_fork_ctx *ctx)
{
	ctx->pids = malloc(sizeof(pid_t) * ctx->n_cmds);
	if (!ctx->pids)
	{
		close_all_pipes(ctx->pipes, ctx->n_pipes);
		free(ctx->pipes);
		return (1);
	}
	return (0);
}

static void	cleanup_pipes(t_fork_ctx *ctx)
{
	close_all_pipes(ctx->pipes, ctx->n_pipes);
	free(ctx->pipes);
	ctx->pipes = NULL;
}

static int	handle_fork_loop_error(t_fork_ctx *ctx)
{
	close_all_pipes(ctx->pipes, ctx->n_pipes);
	free(ctx->pipes);
	free(ctx->pids);
	return (1);
}

static int	fork_loop(t_cmd *start, size_t n_cmds, char **envp,
		t_exec_state *state)
{
	t_fork_ctx	ctx;

	ctx.envp = envp;
	ctx.state = state;
	ctx.n_cmds = n_cmds;
	if (setup_pipes(&ctx, n_cmds) != 0)
		return (1);
	if (alloc_pids_in_ctx(&ctx) != 0)
		return (1);
	if (fork_and_init_children(start, n_cmds, &ctx, ctx.pids) != 0)
		return (handle_fork_loop_error(&ctx));
	cleanup_pipes(&ctx);
	return (wait_and_cleanup(ctx.pids, n_cmds));
}

int	fork_and_run_pipeline(t_cmd *start, size_t n_cmds, char **envp,
		t_exec_state *state)
{
	return (fork_loop(start, n_cmds, envp, state));
}
