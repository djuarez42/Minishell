/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 17:34:49 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 23:11:48 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <string.h> // For memset

void	handle_redirections_out(const char *filename, int *error)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open (redirect out)");
		*error = 1;
		return ;
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2 (redirect out)");
		close (fd);
		*error = 1;
		return ;
	}
}

void	handle_redirections_in(const char *filename, int *error)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("open (redirect in)");
		*error = 1;
		return ;
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2 (redirect in)");
		close(fd);
		*error = 1;
		return ;
	}
}

void	handle_redirections_append(const char *filename, int *error)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("open (redirect append)");
		*error = 1;
		return ;
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2 (redirect append)");
		close(fd);
		*error = 1;
		return ;
	}
}

int	handle_redirections_heredoc(const char *delimiter, bool quoted,
			char **envp, t_heredoc_args *args)
{
	int	fd;
	int	res;

	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);
	args->fd = fd;
	args->delimiter = delimiter;
	args->quoted = quoted;
	args->envp = envp;
	res = write_heredoc_lines(args);
	close(fd);
	if (res == 130)
		return (130);
	redirect_stdin_heredoc(args->heredoc_path);
	return (0);
}

int	handle_redirections(t_redir *redir, char **envp, t_exec_state *state)
{
	int			res;
	int			err;
	t_heredoc_args	args;

	err = 0;
	memset(&args, 0, sizeof(args));
	while (redir)
	{
		if (redir->type == TOKEN_REDIRECT_OUT)
			handle_redirections_out(redir->file, &err);
		else if (redir->type == TOKEN_REDIRECT_IN)
			handle_redirections_in(redir->file, &err);
		else if (redir->type == TOKEN_APPEND)
			handle_redirections_append(redir->file, &err);
		else if (redir->type == TOKEN_HEREDOC)
		{
			args.state = state;
			res = handle_redirections_heredoc(redir->file, redir->quoted, envp, &args);
			if (res == 130)
				return (130);
			if (res != 0)
				err = 1;
		}
		if (err)
			return (1);
		redir = redir->next;
	}
	return (0);

		if (err)
			return (1);
		redir = redir->next;
	
	return (0);
}
