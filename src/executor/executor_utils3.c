/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:48:52 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 18:55:05 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	count_pipeline_cmds(t_cmd *start)
{
	size_t	count;

	count = 0;
	while (start)
	{
		count++;
		if (start->pipe == 0)
			break ;
		start = start->next;
	}
	return (count);
}

static int	create_pipes_loop(int (*pipes)[2], size_t n_pipes)
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

int	create_pipes(int (**pipes)[2], size_t n_pipes)
{
	if (n_pipes == 0)
	{
		*pipes = NULL;
		return (0);
	}
	*pipes = (int (*)[2])malloc(sizeof(int [2]) * n_pipes);
	if (!*pipes)
		return (-1);
	return (create_pipes_loop(*pipes, n_pipes));
}

void	close_all_pipes(int (*pipes)[2], size_t n_pipes)
{
	size_t	i;

	i = 0;
	while (i < n_pipes)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

void	wire_child_pipes(size_t idx, size_t n_cmds, int (*pipes)[2])
{
	if (n_cmds <= 1)
		return ;
	if (idx > 0)
	{
		if (dup2(pipes[idx - 1][0], STDIN_FILENO) == -1)
			perror("dup2 stdin");
	}
	if (idx < n_cmds - 1)
	{
		if (dup2(pipes[idx][1], STDOUT_FILENO) == -1)
			perror("dup2 stdout");
	}
}
