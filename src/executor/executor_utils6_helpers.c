/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils6_helpers.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 20:11:00 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/09/27 20:11:00 by ekakhmad        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_in_path_dirs_helper(char *cmd, char **paths)
{
	char		*full_path;
	int			i;
	struct stat	sb;

	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin_free(full_path, cmd);
		if (stat(full_path, &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
			{
				free(full_path);
				i++;
				continue ;
			}
			if (access(full_path, X_OK) == 0)
			{
				free_split(paths);
				return (full_path);
			}
		}
		else
			free(full_path);
		i++;
	}
	free_split(paths);
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
