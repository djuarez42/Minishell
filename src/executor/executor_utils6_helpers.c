/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils6_helpers.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 21:06:04 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 21:23:08 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*try_path(char *dir, char *cmd, char **paths);

char	*search_in_path_dirs_helper(char *cmd, char **paths)
{
	char	*full_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		full_path = try_path(paths[i], cmd, paths);
		if (full_path)
			return (full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

static char	*try_path(char *dir, char *cmd, char **paths)
{
	char		*full_path;
	struct stat	sb;

	full_path = ft_strjoin(dir, "/");
	full_path = ft_strjoin_free(full_path, cmd);
	if (stat(full_path, &sb) == 0)
	{
		if (S_ISDIR(sb.st_mode))
		{
			free(full_path);
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
		{
			free_split(paths);
			return (full_path);
		}
	}
	else
		free(full_path);
	return (NULL);
}

void	handle_execve_error_helper(char *exec_path, char **argv)
{
	int	code;

	if (errno == ENOENT)
	{
		print_execve_error(argv[0]);
		code = 127;
	}
	else if (errno == EACCES || errno == EPERM || errno == ENOEXEC)
	{
		print_execve_error(exec_path);
		code = 126;
	}
	else if (errno == EISDIR)
	{
		print_execve_error(exec_path);
		code = 126;
	}
	else
	{
		print_execve_error(exec_path);
		code = 1;
	}
	_exit(code);
}
