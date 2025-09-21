/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:42:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 16:24:40 by djuarez          ###   ########.fr       */
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

static int	create_pipes(int (**pipes)[2], size_t n_pipes)
{
	size_t	i;

	*pipes = NULL;
	if (n_pipes == 0)
		return (0);
	*pipes = (int (*)[2])malloc(sizeof(int [2]) * n_pipes);
	if (!*pipes)
		return (-1);
	i = 0;
	while (i < n_pipes)
	{
		if (pipe((*pipes)[i]) == -1)
		{
			while (i > 0)
			{
				close((*pipes)[i - 1][0]);
				close((*pipes)[i - 1][1]);
				i--;
			}
			free(*pipes);
			*pipes = NULL;
			return (-1);
		}
		i++;
	}
	return (0);
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

static int	wait_pipeline(pid_t *pids, size_t n)
{
	int		status;
	pid_t	w;
	pid_t	last_pid;
	int		final_status;
	size_t	left;

	status = 0;
	w = 0;
	last_pid = pids[n - 1];
	final_status = 0;
	left = n;
	while (left > 0)
	{
		w = waitpid(-1, &status, 0);
		if (w == -1)
		{
			if (errno == EINTR)
				continue ;
			perror("waitpid");
			break ;
		}
		if (w == last_pid)
		{
			if (WIFEXITED(status))
				final_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				final_status = 128 + WTERMSIG(status);
		}
		left--;
	}
	return (final_status);
}

static int	run_pipeline(t_cmd *start, size_t n_cmds, char **envp,
	t_exec_state *state)
{
	int				(*pipes)[2];
	size_t			n_pipes;
	pid_t			*pids;
	size_t			i;
	t_cmd			*cur;
	int				res;
	int				code;
	t_exec_state	child_state;

	pipes = NULL;
	if (n_cmds > 1)
		n_pipes = n_cmds - 1;
	else
		n_pipes = 0;
	if (create_pipes(&pipes, n_pipes) == -1)
		return (perror("pipe"), 1);
	pids = malloc(sizeof(pid_t) * n_cmds);
	if (!pids)
	{
		close_all_pipes(pipes, n_pipes);
		free(pipes);
		return (1);
	}
	i = 0;
	cur = start;
	while (i < n_cmds && cur)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			perror("fork");
			close_all_pipes(pipes, n_pipes);
			while (i > 0)
				waitpid(pids[--i], NULL, 0);
			free(pipes);
			free(pids);
			return (1);
		}
		if (pids[i] == 0)
		{
			child_state = *state;
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			wire_child_pipes(i, n_cmds, pipes);
			close_all_pipes(pipes, n_pipes);
			res = handle_redirections_and_quotes(cur->redirs, envp, &child_state);
			if (res == 130)
				exit(130);
			else if (res == 1)
				exit(1);
			if (!cur->argv || !cur->argv[0])
				exit(2);
			if (is_builtin(cur->argv[0]))
				exit(run_builtin_in_child(cur, &envp));
			code = execute_command(NULL, cur, envp);
			exit(code);
		}
		i++;
		cur = cur->next;
	}
	close_all_pipes(pipes, n_pipes);
	free(pipes);
	i = wait_pipeline(pids, n_cmds);
	free(pids);
	return (i);
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
