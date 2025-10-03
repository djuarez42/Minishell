/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils12.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 13:57:43 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	write_heredoc_content_lines(int fd, char **heredoc_content,
		t_heredoc_args *args)
{
	int		i;
	char	*expanded_line;

	i = 0;
	while (heredoc_content[i])
	{
		expanded_line = process_heredoc_line(heredoc_content[i], args);
		if (!expanded_line)
		{
			close(fd);
			return (1);
		}
		write(fd, expanded_line, ft_strlen(expanded_line));
		write(fd, "\n", 1);
		free(expanded_line);
		i++;
	}
	return (0);
}

int	handle_redirections_heredoc_with_content(char **heredoc_content,
		t_heredoc_args *args)
{
	int		fd;
	int		err;

	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);
	err = write_heredoc_content_lines(fd, heredoc_content, args);
	close(fd);
	if (err != 0)
		return (err);
	redirect_stdin_heredoc(args->heredoc_path);
	return (0);
}

static int	handle_single_heredoc(t_redir *redir, char **envp,
		t_exec_state *state, t_heredoc_args *args)
{
	int	res;

	args->quoted = redir->quoted;
	args->envp = envp;
	args->state = state;
	if (redir->heredoc_content && redir->heredoc_content[0] != NULL)
		res = handle_redirections_heredoc_with_content(redir->heredoc_content,
				args);
	else
		res = handle_redirections_heredoc(redir->file, args->quoted, args->envp,
				args);
	if (res == 130)
		return (130);
	if (res != 0)
		return (1);
	return (0);
}

static int	handle_single_redirection(t_redir *redir, char **envp,
		t_exec_state *state, t_heredoc_args *args)
{
	int	err;

	err = 0;
	if (redir->type == TOKEN_REDIRECT_OUT)
		handle_redirections_out(redir->file, &err);
	else if (redir->type == TOKEN_REDIRECT_IN)
		handle_redirections_in(redir->file, &err);
	else if (redir->type == TOKEN_APPEND)
		handle_redirections_append(redir->file, &err, state);
	else if (redir->type == TOKEN_HEREDOC)
		return (handle_single_heredoc(redir, envp, state, args));
	return (err);
}

int	handle_redirections(t_redir *redir, char **envp, t_exec_state *state)
{
	int				err;
	t_heredoc_args	args;

	err = 0;
	ft_memset(&args, 0, sizeof(args));
	while (redir)
	{
		err = handle_single_redirection(redir, envp, state, &args);
		if (err == 130)
			return (130);
		if (err != 0)
			return (1);
		redir = redir->next;
	}
	return (0);
}
