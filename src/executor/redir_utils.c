/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 17:34:49 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/18 20:02:10 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_redirections_out(const char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open (redirect out)");
		exit(1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2 (redirect out)");
		close (fd);
		exit(1);
	}
}

void	handle_redirections_in(const char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror("open (redirect in)");
		exit (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2 (redirect in)");
		close(fd);
		exit(1);
	}
}

void	handle_redirections_append(const char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("open (redirect append)");
		exit (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2 (redirect append)");
		close(fd);
		exit(1);
	}
}

int	handle_redirections_heredoc(const char *delimiter, bool quoted,
			char **envp, t_heredoc_args *args)
{
	int	fd;
	int	res;

	fd = open_heredoc_file();
	args->fd = fd;
	args->delimiter = delimiter;
	args->quoted = quoted;
	args->envp = envp;
	res = write_heredoc_lines(args);
	close(fd);
	if (res == 130)
		return (130);
	redirect_stdin_heredoc();
	return (0);
}

int	handle_redirections(t_redir *redir, char **envp)
{
	int			res;
	t_heredoc_args	args;

	while (redir)
	{
		if (redir->type == TOKEN_REDIRECT_OUT)
			handle_redirections_out(redir->file);
		else if (redir->type == TOKEN_REDIRECT_IN)
			handle_redirections_in(redir->file);
		else if (redir->type == TOKEN_APPEND)
			handle_redirections_append(redir->file);
		else if (redir->type == TOKEN_HEREDOC)
		{
			res = handle_redirections_heredoc(redir->file, redir->quoted, envp,
					&args);
			if (res == 130)
				return (130);
		}
		redir = redir->next;
	}
	return (0);
}
