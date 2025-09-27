/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils14.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 17:26:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 14:25:58 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*process_heredoc_line(const char *line, t_heredoc_args *args)
{
	char	*expanded;

	if (!args->quoted)
		expanded = expand_variables(line, args->envp, args->state, QUOTE_NONE);
	else
		expanded = ft_strdup(line);
	return (expanded);
}

static int	write_single_line(t_heredoc_args *args)
{
	char	*line;
	char	*expanded_line;
	const char	*cmp;
	int		interactive;

	interactive = isatty(STDIN_FILENO);
	line = read_heredoc_line(interactive);
	if (!line)
		return (130);
	/* For unquoted heredoc, bash expands the body. The delimiter match
	   is done against the expanded line; for quoted, match raw line. */
	if (!args->quoted)
	{
		expanded_line = process_heredoc_line(line, args);
		cmp = expanded_line ? expanded_line : line;
	}
	else
	{
		expanded_line = NULL;
		cmp = line;
	}
	if (ft_strncmp(cmp, args->delimiter, ft_strlen(args->delimiter)) == 0
		&& cmp[ft_strlen(args->delimiter)] == '\0')
	{
		free(line);
		if (expanded_line)
			free(expanded_line);
		return (0);
	}
	/* Not a delimiter: write the (possibly expanded) line */
	if (!args->quoted)
	{
		if (expanded_line)
		{
			write(args->fd, expanded_line, ft_strlen(expanded_line));
			free(expanded_line);
		}
		else
			write(args->fd, line, ft_strlen(line));
	}
	else
		write(args->fd, line, ft_strlen(line));
	free(line);
	write(args->fd, "\n", 1);
	return (-1);
}

int	write_heredoc_lines(t_heredoc_args *args)
{
	int		result;
	void	(*original_sigint)(int);
	void	(*original_sigquit)(int);
	int		line_res;

	result = 0;
	original_sigint = signal(SIGINT, SIG_DFL);
	original_sigquit = signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		line_res = write_single_line(args);
		if (line_res >= 0)
		{
			result = line_res;
			break ;
		}
	}
	signal(SIGINT, original_sigint);
	signal(SIGQUIT, original_sigquit);
	return (result);
}

void	redirect_stdin_heredoc(const char *filepath)
{
	int	fd;

	fd = open(filepath, O_RDONLY);
	if (fd == -1 || dup2(fd, STDIN_FILENO) == -1)
	{
		perror("heredoc redirect");
		if (fd != -1)
			close(fd);
		return ;
	}
	close(fd);
}

void	print_error_file(const char *context)
{
	char	*err_msg;

	write(2, "minishell: line 1: ", 20);
	if (context)
		write(2, context, strlen(context));
	write(2, ": ", 2);
	err_msg = strerror(errno);
	if (err_msg)
		write(2, err_msg, strlen(err_msg));
	write(2, "\n", 1);
}
