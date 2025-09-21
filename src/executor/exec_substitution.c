/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_substitution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 22:41:32 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 22:41:43 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*execute_command_substitution(const char *cmd, char **envp)
{
	int		fd[2];
	pid_t	pid;
	char	buffer[1024];
	int		n;
	char	*result;
	size_t	size;
	char	*argv[] = {"/bin/sh", "-c", (char *)cmd, NULL};
	char	*tmp;

	result = NULL;
	size = 0;
	if (pipe(fd) == -1)
		return (NULL);
	pid = fork();
	if (pid < 0)
	{
		close(fd[0]);
		close(fd[1]);
		return (NULL);
	}
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[1], STDERR_FILENO);
		close(fd[1]);
		execve("/bin/sh", argv, envp);
		_exit(127);
	}
	else
	{
		close(fd[1]);
		while ((n = read(fd[0], buffer, sizeof(buffer))) > 0)
		{
			tmp = realloc(result, size + n + 1);
			if (!tmp)
			{
				free(result);
				result = NULL;
				break ;
			}
			result = tmp;
			memcpy(result + size, buffer, n);
			size += n;
		}
		if (result)
			result[size] = '\0';
		close(fd[0]);
		waitpid(pid, NULL, 0);
	}
	return (result);
}

