/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:36:38 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 20:58:46 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_wait_ctx(t_wait_ctx *ctx, pid_t *pids, size_t n)
{
	ctx->status = 0;
	ctx->w = 0;
	ctx->last_pid = pids[n - 1];
	ctx->final_status = 0;
	ctx->left = n;
}

static void	update_final_status(t_wait_ctx *ctx)
{
	if (WIFEXITED(ctx->status))
		ctx->final_status = WEXITSTATUS(ctx->status);
	else if (WIFSIGNALED(ctx->status))
		ctx->final_status = 128 + WTERMSIG(ctx->status);
}

static int	wait_single_process(t_wait_ctx *ctx)
{
	ctx->w = waitpid(-1, &ctx->status, 0);
	if (ctx->w == -1)
	{
		if (errno == EINTR)
			return (-2);
		perror("waitpid");
		return (-1);
	}
	if (ctx->w == ctx->last_pid)
		update_final_status(ctx);
	ctx->left--;
	return (0);
}

int	wait_pipeline(pid_t *pids, size_t n)
{
	t_wait_ctx	ctx;
	int			res;

	init_wait_ctx(&ctx, pids, n);
	while (ctx.left > 0)
	{
		res = wait_single_process(&ctx);
		if (res == -1)
			break ;
	}
	return (ctx.final_status);
}

int	init_pipes_and_n(int n_cmds, int (**pipes)[2], size_t *n_pipes)
{
	*pipes = NULL;
	if (n_cmds > 1)
		*n_pipes = n_cmds - 1;
	else
		*n_pipes = 0;
	if (create_pipes(pipes, *n_pipes) == -1)
	{
		perror("pipe");
		return (1);
	}
	return (0);
}
