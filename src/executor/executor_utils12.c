/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils12.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 04:03:41 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc_lines(char **heredoc_content, int fd,
								t_heredoc_args *args)
{
	int		i;
	char	*expanded_line;

	i = 0;
	while (heredoc_content[i])
	{
		if (!args->quoted)
			expanded_line = expand_variables(heredoc_content[i],
					args->envp, args->state, QUOTE_NONE);
		else
			expanded_line = ft_strdup(heredoc_content[i]);
		if (!expanded_line)
			return (1);
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
	int	fd;

	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);
	if (process_heredoc_lines(heredoc_content, fd, args))
	{
		close(fd);
		return (1);
	}
	close(fd);
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
	if (redir->heredoc_content)
		res = handle_redirections_heredoc_with_content(
				redir->heredoc_content, args);
	else
		res = handle_redirections_heredoc(
				redir->file, args->quoted, args->envp, args);
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
		handle_redirections_append(redir->file, &err);
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
