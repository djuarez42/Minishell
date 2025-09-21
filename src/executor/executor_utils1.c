/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 16:15:03 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 18:43:11 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_child_process_with_ctx(t_fork_ctx *ctx)
{
	t_exec_state	child_state;
	int				res;
	int				code;

	child_state = *(ctx->state);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	wire_child_pipes(ctx->i, ctx->n_cmds, ctx->pipes);
	close_all_pipes(ctx->pipes, ctx->n_pipes);
	res = handle_redirections_and_quotes(ctx->cur->redirs,
			ctx->envp, &child_state);
	if (res == 130)
		exit(130);
	if (res == 1)
		exit(1);
	if (!ctx->cur->argv || !ctx->cur->argv[0])
		exit(2);
	if (is_builtin(ctx->cur->argv[0]))
		exit(run_builtin_in_child(ctx->cur, &ctx->envp));
	code = execute_command(NULL, ctx->cur, ctx->envp);
	exit(code);
}

static int	handle_fork_error(pid_t *pids, size_t i, int (*pipes)[2],
	size_t n_pipes)
{
	while (i > 0)
		waitpid(pids[--i], NULL, 0);
	close_all_pipes(pipes, n_pipes);
	free(pipes);
	free(pids);
	perror("fork");
	return (1);
}

int	wait_and_cleanup(pid_t *pids, size_t n_cmds)
{
	int	status;

	status = wait_pipeline(pids, n_cmds);
	free(pids);
	return (status);
}

int	fork_and_init_children(t_cmd *start, size_t n_cmds, t_fork_ctx *ctx,
		pid_t *pids)
{
	size_t	i;
	t_cmd	*cur;

	i = 0;
	cur = start;
	while (i < n_cmds && cur)
	{
		pids[i] = fork();
		if (pids[i] == -1)
			return (handle_fork_error(pids, i, ctx->pipes, ctx->n_pipes));
		if (pids[i] == 0)
		{
			ctx->cur = cur;
			ctx->i = i;
			run_child_process_with_ctx(ctx);
		}
		i++;
		cur = cur->next;
	}
	return (0);
}

int	setup_pipes(t_fork_ctx *ctx, size_t n_cmds)
{
	if (n_cmds > 1)
		ctx->n_pipes = n_cmds - 1;
	else
		ctx->n_pipes = 0;
	if (create_pipes(&ctx->pipes, ctx->n_pipes) == -1)
		return (perror("pipe"), 1);
	return (0);
}
