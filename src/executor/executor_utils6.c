/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils6.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:28:43 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/29 18:24:11 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*search_in_path_dirs(char *cmd, char **paths)
{
	return (search_in_path_dirs_helper(cmd, paths));
}

char	*find_executable(char *cmd, char **envp)
{
	char	*path_env;
	char	**paths;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = env_get_value(envp, "PATH");
	if (!path_env || path_env[0] == '\0')
		return (NULL);
		/*paths = ft_split("/usr/bin:/bin", ':');*/
	else
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
	handle_execve_error_helper(exec_path, argv);
}

int	execute_execve(char *exec_path, char **argv, char **envp)
{
	update_last_arg_var(exec_path, envp);
	if (execve(exec_path, argv, envp) == -1)
		handle_execve_error(exec_path, argv);
	return (0);
}
