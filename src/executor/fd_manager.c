/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_manager.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 18:36:24 by ekakhmad          #+#    #+#             */
/*   Updated: 2023/08/31 19:22:11 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fd_manager.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

t_fd_state	*fd_state_init(void)
{
	t_fd_state	*state;

	state = (t_fd_state *)malloc(sizeof(t_fd_state));
	if (!state)
		return (NULL);
	state->original_stdin = -1;
	state->original_stdout = -1;
	state->original_stderr = -1;
	state->stdin_modified = false;
	state->stdout_modified = false;
	state->stderr_modified = false;
	return (state);
}

void	fd_state_save_original(t_fd_state *state)
{
	if (!state)
		return ;
	
	/* Duplicate the original FDs so we can restore them later */
	state->original_stdin = dup(STDIN_FILENO);
	state->original_stdout = dup(STDOUT_FILENO);
	state->original_stderr = dup(STDERR_FILENO);
}

void	fd_state_restore_all(t_fd_state *state)
{
	if (!state)
		return ;
	
	/* Restore original stdin if it was modified and saved */
	if (state->stdin_modified && state->original_stdin != -1)
	{
		dup2(state->original_stdin, STDIN_FILENO);
		close(state->original_stdin);
		state->original_stdin = -1;
		state->stdin_modified = false;
	}
	
	/* Restore original stdout if it was modified and saved */
	if (state->stdout_modified && state->original_stdout != -1)
	{
		dup2(state->original_stdout, STDOUT_FILENO);
		close(state->original_stdout);
		state->original_stdout = -1;
		state->stdout_modified = false;
	}
	
	/* Restore original stderr if it was modified and saved */
	if (state->stderr_modified && state->original_stderr != -1)
	{
		dup2(state->original_stderr, STDERR_FILENO);
		close(state->original_stderr);
		state->original_stderr = -1;
		state->stderr_modified = false;
	}
}

void	fd_state_cleanup(t_fd_state *state)
{
	if (!state)
		return ;
	
	/* Ensure all file descriptors are closed and restored */
	fd_state_restore_all(state);
	
	/* Free the state structure */
	free(state);
}

int	setup_input_redirection(const char *filename, t_fd_state *state)
{
	int	fd;

	/* Open the input file for reading */
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("minishell: input redirection");
		return (1);
	}
	
	/* If this is the first stdin redirection, save the original */
	if (!state->stdin_modified)
	{
		state->original_stdin = dup(STDIN_FILENO);
		state->stdin_modified = true;
	}
	
	/* Redirect stdin to the file */
	dup2(fd, STDIN_FILENO);
	close(fd);
	
	return (0);
}

int	setup_output_redirection(const char *filename, bool append, t_fd_state *state)
{
	int	fd;
	int	flags;

	/* Set flags for opening the output file */
	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	
	/* Open the output file */
	fd = open(filename, flags, 0644);
	if (fd == -1)
	{
		perror("minishell: output redirection");
		return (1);
	}
	
	/* If this is the first stdout redirection, save the original */
	if (!state->stdout_modified)
	{
		state->original_stdout = dup(STDOUT_FILENO);
		state->stdout_modified = true;
	}
	
	/* Redirect stdout to the file */
	dup2(fd, STDOUT_FILENO);
	close(fd);
	
	return (0);
}

int	setup_heredoc_redirection(int fd, t_fd_state *state)
{
	/* If this is the first stdin redirection, save the original */
	if (!state->stdin_modified)
	{
		state->original_stdin = dup(STDIN_FILENO);
		state->stdin_modified = true;
	}
	
	/* Redirect stdin to the heredoc file descriptor */
	dup2(fd, STDIN_FILENO);
	
	/* The caller is responsible for closing the original fd */
	
	return (0);
}

void	cleanup_redirections_parent(t_fd_state *fd_state)
{
	/* Restore all original file descriptors and free the state */
	if (fd_state)
	{
		fd_state_restore_all(fd_state);
		fd_state_cleanup(fd_state);
	}
}
