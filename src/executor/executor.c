/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:42:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/18 20:30:16 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "libft.h"
#include <signal.h>
#include "builtins.h"
#include <fcntl.h>

static inline void fd_diag_log(const char *where)
{
	int fd = open("/tmp/ms_diag.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1) return;
	char buf[512];
	char link0[256], link1[256], link2[256];
	ssize_t l0 = readlink("/proc/self/fd/0", link0, sizeof(link0)-1);
	ssize_t l1 = readlink("/proc/self/fd/1", link1, sizeof(link1)-1);
	ssize_t l2 = readlink("/proc/self/fd/2", link2, sizeof(link2)-1);
	if (l0>0) link0[l0]=0; else strcpy(link0, "?");
	if (l1>0) link1[l1]=0; else strcpy(link1, "?");
	if (l2>0) link2[l2]=0; else strcpy(link2, "?");
	int flags0 = fcntl(0, F_GETFL);
	int flags1 = fcntl(1, F_GETFL);
	int flags2 = fcntl(2, F_GETFL);
	int len = snprintf(buf, sizeof(buf), "[%s] fd0:%s fl0:%d | fd1:%s fl1:%d | fd2:%s fl2:%d\n", where, link0, flags0, link1, flags1, link2, flags2);
	if (len>0) write(fd, buf, (size_t)len);
	close(fd);
}

/* ---------------- PATH search and exec helpers (existing) ---------------- */
char	*find_executable(char *cmd, char **envp)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = env_get_value(envp, "PATH");
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
		{
			return (free_split(paths), full_path);
		}
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}


int	execute_execve(char *exec_path, char **argv, char **envp)
{
	int		idx;
	char	*new_var;
	int		code;

	/* Update the _ environment variable for execve */
	idx = env_find_index(envp, "_");
	if (idx >= 0)
	{
		free(envp[idx]);
		new_var = ft_strjoin("_=", exec_path);
		envp[idx] = (new_var) ? new_var : ft_strdup("_=");
	}

	if (execve(exec_path, argv, envp) == -1)
	{
		/* Map errno to bash-compatible exit codes and messages */
		if (errno == ENOENT)
		{
			fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
			code = 127;
		}
		else if (errno == EACCES || errno == EPERM || errno == EISDIR || errno == ENOEXEC)
		{
			fprintf(stderr, "minishell: %s: %s\n", exec_path, (errno == EISDIR) ? "Is a directory" : "Permission denied");
			code = 126;
		}
		else
		{
			fprintf(stderr, "minishell: %s: %s\n", exec_path, strerror(errno));
			code = 1;
		}
		_exit(code);
	}
	return (0);
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
	int		status;
	pid_t	w;
	pid_t	last_pid;
	int		final_status;
	size_t	left;

	last_pid = pids[n - 1];
	final_status = 0;
	left = n;
	while (left > 0)
	{
		w = waitpid(-1, &status, 0);
		if (w == -1)
		{
			if (errno == EINTR)
				continue;
			perror("waitpid");
			break;
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

static int run_pipeline(t_cmd *start, size_t n_cmds, char **envp, t_exec_state *state)
{
    int (*pipes)[2] = NULL;
    size_t n_pipes = (n_cmds > 1) ? n_cmds - 1 : 0;
    pid_t *pids;
    size_t i;
    t_cmd *cur;
    int res, code;

    if (create_pipes(&pipes, n_pipes) == -1)
        return (perror("pipe"), 1);

    pids = malloc(sizeof(pid_t) * n_cmds);
    if (!pids)
    {
        if (pipes) close_all_pipes(pipes, n_pipes);
        free(pipes);
        return 1;
    }

    // Ya no hacemos expand_cmd_inplace, argv ya está expandido por parser

    i = 0;
    cur = start;
    while (i < n_cmds && cur)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            if (pipes) close_all_pipes(pipes, n_pipes);
            while (i > 0)
                waitpid(pids[--i], NULL, 0);
            free(pipes);
            free(pids);
            return 1;
        }

        if (pids[i] == 0)
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            wire_child_pipes(i, n_cmds, pipes);
            if (pipes) close_all_pipes(pipes, n_pipes);

            t_exec_state child_state = *state;
            res = handle_redirections_and_quotes(cur->redirs, envp, &child_state);
            if (res == 130) exit(130);
            else if (res == 1) exit(1);

            if (!cur->argv || !cur->argv[0]) exit(2);

            if (is_builtin(cur->argv[0]))
                exit(run_builtin_in_child(cur, &envp));

            code = execute_command(NULL, cur, envp);
            exit(code);
        }
        i++;
        cur = cur->next;
    }

    if (pipes) close_all_pipes(pipes, n_pipes);
    free(pipes);
    i = wait_pipeline(pids, n_cmds);
    free(pids);
    return i;
}

/* ----------------------------- Entry point ------------------------------ */
void	executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)
{
	/* DIAG */ { int fd = open("./ms_diag.log", O_WRONLY|O_CREAT|O_APPEND, 0644); if (fd!=-1){ write(fd, "EXECUTOR ENTER\n", 15); close(fd);} }
	t_cmd	*cur;
	size_t	n;
	int		status;
	char	**envp;
	int		res;

	envp = *penvp;
	cur = cmd_list;
	while (cur)
	{
		if (!cur->argv || !cur->argv[0])
		{
			fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
			state->last_status = 2;
			cur = cur->next;
			continue;
		}
		n = count_pipeline_cmds(cur);
		// fprintf(stderr, "[DBG] executor: cmd='%s' n=%zu pipe=%d builtin=%d\n", cur->argv[0], n, cur->pipe, is_builtin(cur->argv[0]));
		fd_diag_log("executor:before-branch");
		if (n == 1 && is_builtin(cur->argv[0]) && cur->pipe == 0)
		{
			int save_in;
			int save_out;
			int save_err;

			/* TEMP DBG */ // write(STDERR_FILENO, "[DBG] parent builtin path\n", 25);
			
			// Ensure all output is flushed before saving FDs
			fflush(stdout);
			fflush(stderr);
			
			save_in = dup(STDIN_FILENO);
			save_out = dup(STDOUT_FILENO);
			save_err = dup(STDERR_FILENO);

			fd_diag_log("executor:parent-builtin:before-redirs");
			res = handle_redirections_and_quotes(cur->redirs, envp, state);
			fd_diag_log("executor:parent-builtin:after-redirs");
			
			/* TEMP DBG */ // dprintf(STDERR_FILENO, "[DBG] parent after redirs res=%d\n", res);
			
			if (res == 130)
				status = 130;
			else if (res == 1)
				status = 1;
			else {
				// Explicitly flush before running builtin
				fflush(stdout);
				fflush(stderr);
				
				status = run_builtin_in_parent(cur, &envp);
				
				// Explicitly flush after running builtin
				fflush(stdout);
				fflush(stderr);
			}
			
			/* TEMP DBG */ // dprintf(STDERR_FILENO, "[DBG] parent builtin status=%d\n", status);
			
			// Flush any pending output to redirected files before restoring FDs
			fflush(stdout);
			fflush(stderr);
			
			if (save_in != -1)
				dup2(save_in, STDIN_FILENO);
			if (save_out != -1)
				dup2(save_out, STDOUT_FILENO);
			if (save_err != -1)
				dup2(save_err, STDERR_FILENO);
				
			// Flush again after restoring FDs
			fflush(stdout);
			fflush(stderr);
			
			if (save_in != -1)
				close(save_in);
			if (save_out != -1)
				close(save_out);
			if (save_err != -1)
				close(save_err);
		}
		else
		{
			// fprintf(stderr, "[DBG] pipeline or external path: n=%zu\n", n);
			status = run_pipeline(cur, n, envp, state);
			// fprintf(stderr, "[DBG] pipeline/external status=%d\n", status);
		}
		state->last_status = status;
		while (n-- > 0 && cur)
			cur = cur->next;
	}
	*penvp = envp;
}
