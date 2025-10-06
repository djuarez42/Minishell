/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils15.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:15 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/06 15:28:11 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	get_fake_pid(void)
{
	static int	fake_pid;

	fake_pid = 1000;
	fake_pid++;
	return (fake_pid);
}

static char	*try_open_heredoc(int *out_fd, int *counter, char *count_str)
{
	char	*path;

	itoa_buffer_int(get_fake_pid(), count_str);
	path = ft_strjoin("/tmp/.heredoc_", count_str);
	itoa_buffer_int((*counter)++, count_str);
	path = ft_strjoin_free(path, "_");
	path = ft_strjoin_free(path, count_str);
	*out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (*out_fd != -1)
		return (path);
	free(path);
	return (NULL);
}

static char	*try_open_heredoc_fallback(int *out_fd, int *counter,
			char *count_str)
{
	char	*path;
	char	*tmp;

	itoa_buffer_int(get_fake_pid(), count_str);
	path = ft_strjoin(".heredoc_", count_str);
	itoa_buffer_int((*counter)++, count_str);
	tmp = ft_strjoin_free(path, "_");
	path = ft_strjoin_free(tmp, count_str);
	*out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (*out_fd == -1)
	{
		free(path);
		return (NULL);
	}
	return (path);
}

int	open_heredoc_file(t_heredoc_args *args)
{
	int			fd;
	char		*path;
	static int	counter;
	char		count_str[16];

	counter = 0;
	path = try_open_heredoc(&fd, &counter, count_str);
	if (path)
	{
		args->heredoc_path = path;
		return (fd);
	}
	path = try_open_heredoc_fallback(&fd, &counter, count_str);
	if (!path)
	{
		print_errno("open heredoc");
		args->heredoc_path = NULL;
		return (-1);
	}
	args->heredoc_path = path;
	return (fd);
}
