/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_substitution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 22:41:32 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/22 19:11:14 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static pid_t	spawn_command_subst(const char *cmd, char **envp, int *read_fd)
{
	int		fd[2];
	pid_t	pid;
	char	*argv[4];

	argv[0] = "/bin/sh";
	argv[1] = "-c";
	argv[2] = (char *)cmd;
	argv[3] = NULL;
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid < 0)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
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
	close(fd[1]);
	*read_fd = fd[0];
	return (pid);
}

static char	*read_command_output_and_wait(int read_fd, pid_t pid)
{
	char	buffer[1024];
	ssize_t	n;
	char	*result;
	size_t	size;
	char	*tmp;

	size = 0;
	result = NULL;
	n = read(read_fd, buffer, sizeof(buffer));
	while (n > 0)
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
		n = read(read_fd, buffer, sizeof(buffer));
	}
	if (result)
		result[size] = '\0';
	close(read_fd);
	waitpid(pid, NULL, 0);
	return (result);
}

char	*execute_command_subst(const char *cmd, char **envp)
{
	int		read_fd;
	pid_t	pid;

	pid = spawn_command_subst(cmd, envp, &read_fd);
	if (pid == -1)
		return (NULL);
	return (read_command_output_and_wait(read_fd, pid));
}
