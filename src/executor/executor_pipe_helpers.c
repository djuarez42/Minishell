/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 20:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:57:36 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	create_pipes_impl(int (*pipes)[2], size_t n_pipes)
{
	size_t	i;

	i = 0;
	while (i < n_pipes)
	{
		if (pipe(pipes[i]) == -1)
		{
			while (i > 0)
			{
				i--;
				close(pipes[i][0]);
				close(pipes[i][1]);
			}
			return (-1);
		}
		i++;
	}
	return (0);
}

int	fd_guard_begin(int saved[3])
{
	saved[0] = dup(STDIN_FILENO);
	saved[1] = dup(STDOUT_FILENO);
	saved[2] = dup(STDERR_FILENO);
	if (saved[0] == -1 || saved[1] == -1 || saved[2] == -1)
		return (-1);
	return (0);
}

void	fd_guard_end(int saved[3])
{
	if (saved[0] != -1)
		dup2(saved[0], STDIN_FILENO);
	if (saved[1] != -1)
		dup2(saved[1], STDOUT_FILENO);
	if (saved[2] != -1)
		dup2(saved[2], STDERR_FILENO);
	if (saved[0] != -1)
		close(saved[0]);
	if (saved[1] != -1)
		close(saved[1]);
	if (saved[2] != -1)
		close(saved[2]);
}
