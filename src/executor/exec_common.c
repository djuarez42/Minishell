/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_common.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 22:28:41 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 22:41:12 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_split(char **split)
{
	int	i;

	i = 0;
	while (split && split[i])
		free(split[i++]);
	free(split);
}

char	*str_append(char *base, const char *add)
{
	char	*new;
	size_t	len;

	len = 0;
	if (base)
		len += ft_strlen(base);
	if (add)
		len += ft_strlen(add);
	new = malloc(len + 1);
	if (!new)
		return (NULL);
	new[0] = '\0';
	if (base)
		ft_strlcat(new, base, len + 1);
	if (add)
		ft_strlcat(new, add, len + 1);
	free(base);
	return (new);
}

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
			return (free_split(paths), full_path);
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

static void	report_exec_error(const char *exec_path, char **argv)
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
	if (execve(exec_path, argv, envp) == -1)
		report_exec_error(exec_path, argv);
	return (0);
}

/* execute_command moved to exec_run.c to satisfy function count */
