/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 17:34:49 by djuarez           #+#    #+#             */
/*   Updated: 2025/07/26 17:35:21 by djuarez          ###   ########.fr       */
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

void	handle_redirections_heredoc(const char	*delimiter)
{
	char	*line;
	int		fd;

	fd = open("/tmp/.heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (perror("open heredoc"), exit(1), (void)0);
	while (1)
	{
		line = readline("> ");
		if (!line || (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
				&& line[ft_strlen(delimiter)] == '\0'))
			break ;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	fd = open("/tmp/.heredoc_tmp", O_RDONLY);
	if (fd == -1 || dup2(fd, STDIN_FILENO) == -1)
		return (perror("heredoc redirect"), close(fd), exit(1), (void)0);
	close(fd);
}

void	handle_redirections(t_redir *redir)
{
	while (redir)
	{
		if (redir->type == TOKEN_REDIRECT_OUT)
			handle_redirections_out(redir->file);
		else if (redir->type == TOKEN_REDIRECT_IN)
			handle_redirections_in(redir->file);
		else if (redir->type == TOKEN_APPEND)
			handle_redirections_append(redir->file);
		else if (redir->type == TOKEN_HEREDOC)
			handle_redirections_heredoc(redir->file);
		redir = redir->next;
	}
}
