/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils7.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:38:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/06 14:11:54 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exec_subst_child(const char *cmd, char **envp, int fd[2])
{
	char	*argv[4];

	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	dup2(fd[1], STDERR_FILENO);
	close(fd[1]);
	argv[0] = "/bin/sh";
	argv[1] = "-c";
	argv[2] = (char *)cmd;
	argv[3] = NULL;
	execve("/bin/sh", argv, envp);
	exit(127);
}

static char	*append_buffer(char *result, size_t size,
							const char *buffer, int n)
{
	char	*new_buf;

	new_buf = malloc(size + n + 1);
	if (!new_buf)
	{
		free(result);
		return (NULL);
	}
	if (result)
		ft_memcpy(new_buf, result, size);
	ft_memcpy(new_buf + size, buffer, n);
	free(result);
	return (new_buf);
}

static char	*read_from_pipe(int fd)
{
	char	buffer[1024];
	int		n;
	char	*result;
	size_t	size;

	result = NULL;
	size = 0;
	n = read(fd, buffer, sizeof(buffer));
	while (n > 0)
	{
		result = append_buffer(result, size, buffer, n);
		if (!result)
			break ;
		size += n;
		n = read(fd, buffer, sizeof(buffer));
	}
	if (result)
		result[size] = '\0';
	return (result);
}

static char	*exec_subst_parent(pid_t pid, int fd[2])
{
	char	*result;

	close(fd[1]);
	result = read_from_pipe(fd[0]);
	close(fd[0]);
	waitpid(pid, NULL, 0);
	return (result);
}

char	*execute_command_substitution(const char *cmd, char **envp)
{
	int		fd[2];
	pid_t	pid;

	if (pipe(fd) == -1)
		return (NULL);
	pid = fork();
	if (pid < 0)
		return (NULL);
	else if (pid == 0)
		exec_subst_child(cmd, envp, fd);
	return (exec_subst_parent(pid, fd));
}
