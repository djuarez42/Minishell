/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils13.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 17:34:49 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 19:48:46 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* helpers are implemented in executor_redir_helpers.c */

void	handle_redirections_out(const char *filename, int *error)
{
	int	fd;

	if (!filename)
	{
		errno = EINVAL;
		print_error_file("null filename");
		*error = 1;
		return ;
	}
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		print_error_file(filename);
		*error = 1;
		return ;
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		print_error_file("dup2");
		close(fd);
		*error = 1;
		return ;
	}
	close(fd);
}

void	handle_redirections_in(const char *filename, int *error)
{
	int	fd;

	if (!filename)
	{
		errno = EINVAL;
		print_error_file("null filename");
		*error = 1;
		return ;
	}
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		print_error_file(filename);
		*error = 1;
		return ;
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		print_error_file("dup2");
		close(fd);
		*error = 1;
		return ;
	}
	close(fd);
}

void	handle_redirections_append(const char *filename, int *error,
		t_exec_state *state)
{
	char		expanded[4096];
	const char	*target;

	target = filename;
	if (prepare_append_filename(filename, expanded, &target, state))
	{
		*error = 1;
		return ;
	}
	if (attempt_append_target(target, state))
	{
		*error = 1;
		return ;
	}
}

int	handle_redirections_heredoc(const char *delimiter, bool quoted, char **envp,
		t_heredoc_args *args)
{
	int	fd;
	int	res;

	args->delimiter = delimiter;
	args->quoted = quoted;
	args->envp = envp;
	args->heredoc_path = NULL;
	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);
	args->fd = fd;
	res = write_heredoc_lines(args);
	close(fd);
	if (res == 130)
		return (130);
	redirect_stdin_heredoc(args->heredoc_path);
	return (0);
}
