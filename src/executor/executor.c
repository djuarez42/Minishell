/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:42:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 18:20:02 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	count_pipeline_cmds(t_cmd *start)
{
	size_t	count;

	count = 0;
	while (start)
	{
		count++;
		if (start->pipe == 0)
			break ;
		start = start->next;
	}
	return (count);
}

static int	create_pipes_loop(int (*pipes)[2], size_t n_pipes)
{
	size_t	i;

	i = 0;
	while (i < n_pipes)
	{
		if (pipe(pipes[i]) == -1)
		{
			while (i > 0)
			{
				i--;
				close(pipes[i][0]);
				close(pipes[i][1]);
			}
			return (-1);
		}
		i++;
	}
	return (0);
}

static int	create_pipes(int (**pipes)[2], size_t n_pipes)
{
	if (n_pipes == 0)
	{
		*pipes = NULL;
		return (0);
	}
	*pipes = (int (*)[2])malloc(sizeof(int [2]) * n_pipes);
	if (!*pipes)
		return (-1);
	return (create_pipes_loop(*pipes, n_pipes));
}

static void	close_all_pipes(int (*pipes)[2], size_t n_pipes)
{
	size_t	i;

	i = 0;
	while (i < n_pipes)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static void	wire_child_pipes(size_t idx, size_t n_cmds, int (*pipes)[2])
{
	if (n_cmds <= 1)
		return ;
	if (idx > 0)
	{
		if (dup2(pipes[idx - 1][0], STDIN_FILENO) == -1)
			perror("dup2 stdin");
	}
	if (idx < n_cmds - 1)
	{
		if (dup2(pipes[idx][1], STDOUT_FILENO) == -1)
			perror("dup2 stdout");
	}
}

static void init_wait_ctx(t_wait_ctx *ctx, pid_t *pids, size_t n)
{
	ctx->status = 0;
	ctx->w = 0;
	ctx->last_pid = pids[n - 1];
	ctx->final_status = 0;
	ctx->left = n;
}

static void update_final_status(t_wait_ctx *ctx)
{
	if (WIFEXITED(ctx->status))
		ctx->final_status = WEXITSTATUS(ctx->status);
	else if (WIFSIGNALED(ctx->status))
		ctx->final_status = 128 + WTERMSIG(ctx->status);
}

static int wait_single_process(t_wait_ctx *ctx)
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

static int wait_pipeline(pid_t *pids, size_t n)
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

static int	init_pipes_and_n(int n_cmds, int (**pipes)[2], size_t *n_pipes)
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

static int	wait_and_cleanup(pid_t *pids, size_t n_cmds)
{
	int	status;

	status = wait_pipeline(pids, n_cmds);
	free(pids);
	return (status);
}

static int fork_and_init_children(t_cmd *start, size_t n_cmds, t_fork_ctx *ctx,
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

static int	setup_pipes(t_fork_ctx *ctx, size_t n_cmds)
{
	if (n_cmds > 1)
		ctx->n_pipes = n_cmds - 1;
	else
		ctx->n_pipes = 0;
	if (create_pipes(&ctx->pipes, ctx->n_pipes) == -1)
		return (perror("pipe"), 1);
	return (0);
}

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

static int	fork_and_run_pipeline(t_cmd *start, size_t n_cmds,
	char **envp, t_exec_state *state)
{
	return (fork_loop(start, n_cmds, envp, state));
}

static int	run_pipeline(t_cmd *start, size_t n_cmds, char **envp,
	t_exec_state *state)
{
	int		(*pipes)[2];
	size_t	n_pipes;

	if (init_pipes_and_n(n_cmds, &pipes, &n_pipes) != 0)
		return (1);
	return (fork_and_run_pipeline(start, n_cmds, envp, state));
}

static int	handle_empty_command(t_exec_state *state)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
	state->last_status = 2;
	return (0);
}

static int	apply_redirections_and_run_builtin(t_cmd *cur, char ***envp,
			t_exec_state *state)
{
	int	res;
	int	status;

	res = handle_redirections_and_quotes(cur->redirs, *envp, state);
	if (res == 130)
		status = 130;
	else if (res == 1)
		status = 1;
	else
		status = run_builtin_in_parent(cur, envp);
	return (status);
}

static int	run_parent_builtin(t_cmd *cur, char ***envp, t_exec_state *state)
{
	int	save_in;
	int	save_out;
	int	save_err;
	int	status;

	save_in = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	save_err = dup(STDERR_FILENO);
	status = apply_redirections_and_run_builtin(cur, envp, state);
	if (save_in != -1)
		dup2(save_in, STDIN_FILENO);
	if (save_out != -1)
		dup2(save_out, STDOUT_FILENO);
	if (save_err != -1)
		dup2(save_err, STDERR_FILENO);
	if (save_in != -1)
		close(save_in);
	if (save_out != -1)
		close(save_out);
	if (save_err != -1)
		close(save_err);
	return (status);
}

void	executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)
{
	t_cmd	*cur;
	size_t	n;
	int		status;
	char	**envp;

	envp = *penvp;
	cur = cmd_list;
	while (cur)
	{
		if (!cur->argv || !cur->argv[0])
		{
			handle_empty_command(state);
			cur = cur->next;
			continue ;
		}
		n = count_pipeline_cmds(cur);
		if (n == 1 && is_builtin(cur->argv[0]) && cur->pipe == 0)
			status = run_parent_builtin(cur, &envp, state);
		else
			status = run_pipeline(cur, n, envp, state);
		state->last_status = status;
		while (n-- > 0 && cur)
			cur = cur->next;
	}
	*penvp = envp;
}
