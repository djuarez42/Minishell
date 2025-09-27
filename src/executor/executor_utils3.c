/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:48:52 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 19:57:36 by ekakhmad         ###   ########.fr       */
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

int	create_pipes(int (**pipes)[2], size_t n_pipes)
{
	if (n_pipes == 0)
	{
		*pipes = NULL;
		return (0);
	}
	(*pipes) = malloc (sizeof (int [2]) * n_pipes);
	if (!*pipes)
		return (-1);
	if (create_pipes_impl(*pipes, n_pipes) == -1)
	{
		free(*pipes);
		*pipes = NULL;
		return (-1);
	}
	return (0);
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
			print_errno("dup2 stdin");
	}
	if (idx < n_cmds - 1)
	{
		if (dup2(pipes[idx][1], STDOUT_FILENO) == -1)
			print_errno("dup2 stdout");
	}
}

/* fd guard helpers are implemented in executor_pipe_helpers.c */
