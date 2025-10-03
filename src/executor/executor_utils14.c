/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils14.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 17:26:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 16:59:02 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* helpers moved to executor_heredoc_helpers.c */

static int	write_single_line(t_heredoc_args *args)
{
	char		*line;
	char		*expanded_line;
	const char	*cmp;
	int			interactive;

	interactive = isatty(STDIN_FILENO);
	line = read_heredoc_line(interactive, args->state);
	if (!line)
		return (130);
	cmp = get_cmp_for_heredoc(line, &expanded_line, args);
	if (handle_delimiter_and_cleanup(cmp, args->delimiter,
			line, expanded_line) == 0)
		return (0);
	if (!args->quoted)
	{
		write_line_to_heredoc_fd(args->fd, line, expanded_line);
		if (expanded_line)
			free(expanded_line);
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
		print_errno("heredoc redirect");
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
