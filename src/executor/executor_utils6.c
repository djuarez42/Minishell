/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils6.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:28:43 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 21:45:39 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*search_in_path_dirs(char *cmd, char **paths)
{
	char	*full_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin_free(full_path, cmd);
		if (access(full_path, X_OK) == 0)
		{
			free_split(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

char	*find_executable(char *cmd, char **envp)
{
	if (!cmd || cmd[0] == '\0')
		return (NULL);
	char	*path_env;
	char	**paths;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = env_get_value(envp, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_path_dirs(cmd, paths));
}

static void	update_last_arg_var(char *exec_path, char **envp)
{
	int		idx;
	char	*new_var;

	idx = env_find_index(envp, "_");
	if (idx >= 0)
	{
		free(envp[idx]);
		new_var = ft_strjoin("_=", exec_path);
		if (new_var)
			envp[idx] = new_var;
		else
			envp[idx] = ft_strdup("_=");
	}
}

static void	handle_execve_error(char *exec_path, char **argv)
{
	int	code;

	if (errno == ENOENT)
	{
		fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
		code = 127;
	}
	else if (errno == EACCES || errno == EPERM || errno == EISDIR
		|| errno == ENOEXEC)
	{
		if (errno == EISDIR)
			fprintf(stderr, "minishell: %s: Is a directory\n", exec_path);
		else
			fprintf(stderr, "minishell: %s: Permission denied\n", exec_path);
		code = 126;
	}
	else
	{
		fprintf(stderr, "minishell: %s: %s\n", exec_path, strerror(errno));
		code = 1;
	}
	_exit(code);
}

int	execute_execve(char *exec_path, char **argv, char **envp)
{
	update_last_arg_var(exec_path, envp);
	if (execve(exec_path, argv, envp) == -1)
		handle_execve_error(exec_path, argv);
	return (0);
}
