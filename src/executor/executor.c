/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:42:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/09 14:19:00 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "libft.h"
#include <signal.h>
#include "builtins.h"

/* ---------------- PATH search and exec helpers (existing) ---------------- */
char	*find_executable(char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin_free(full_path, cmd);
		if (access(full_path, X_OK) == 0)
			return (free_split(paths), full_path);
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

void	execute_execve(char *exec_path, char **argv, char **envp)
{
	if (execve(exec_path, argv, envp) == -1)
	{
		perror("execve");
		exit (1);
	}
}

/* --------------------------- Pipeline helpers --------------------------- */
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
	*pipes = (int (*)[2])malloc(sizeof(int[2]) * n_pipes);
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
	size_t	i;
	int		status;
	int		last_status;

	last_status = 0;
	i = 0;
	while (i < n)
	{
		if (waitpid(pids[i], &status, 0) == -1)
			perror("waitpid");
		if (i == n - 1)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last_status);
}

static int	run_pipeline(t_cmd *start, size_t n_cmds, char **envp)
{
	int			(*pipes)[2];
	size_t		n_pipes;
	pid_t		*pids;
	size_t		i;
	t_cmd		*cur;

	pipes = NULL;
	n_pipes = (n_cmds > 1) ? (n_cmds - 1) : 0;
	if (create_pipes(&pipes, n_pipes) == -1)
		return (perror("pipe"), 1);
	pids = (pid_t *)malloc(sizeof(pid_t) * n_cmds);
	if (!pids)
	{
		if (pipes)
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
			// Parent cleanup: close all pipes and wait spawned children
			if (pipes)
				close_all_pipes(pipes, n_pipes);
			while (i > 0)
				waitpid(pids[--i], NULL, 0);
			free(pipes);
			free(pids);
			return (1);
		}
		if (pids[i] == 0)
		{
			// Child process
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			wire_child_pipes(i, n_cmds, pipes);
			if (pipes)
				close_all_pipes(pipes, n_pipes);
			// Apply per-command redirections (override pipe ends if necessary)
			handle_redirections_and_quotes(cur->redirs);
			// Builtins within pipeline run in the child
if (is_builtin(cur->argv[0]))
				exit(run_builtin_in_child(cur, &envp));
			// External command
			execute_command(NULL, cur, envp);
			// If execute_command returns, something went wrong
			exit(127);
		}
		// Parent proceeds to next
		i++;
		cur = cur->next;
	}
	// Parent: close all pipe fds and wait children
	if (pipes)
		close_all_pipes(pipes, n_pipes);
	free(pipes);
	i = wait_pipeline(pids, n_cmds);
	free(pids);
	return (i);
}

/* ----------------------------- Entry point ------------------------------ */
void	executor(t_cmd *cmd_list, char ***penvp)
{
	t_cmd	*cur;
	size_t	n;
	int		status;
	char	**envp;

	envp = *penvp;
	cur = cmd_list;
	while (cur)
	{
		n = count_pipeline_cmds(cur);
		// Single non-piped builtin: run in parent
		if (n == 1 && cur->argv && cur->argv[0] && is_builtin(cur->argv[0]) && cur->pipe == 0)
			status = run_builtin_in_parent(cur, &envp);
		else
			status = run_pipeline(cur, n, envp);
		(void)status; // TODO: store in shell state for $?
		// Advance cur by n commands
		while (n-- > 0 && cur)
			cur = cur->next;
	}
	*penvp = envp;
}
