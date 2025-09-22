/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils14.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 17:26:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 04:44:30 by djuarez          ###   ########.fr       */
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

	line = readline("> ");
	if (!line)
		return (130);
	if (ft_strncmp(line, args->delimiter, ft_strlen(args->delimiter)) == 0
		&& line[ft_strlen(args->delimiter)] == '\0')
	{
		free(line);
		return (0);
	}
	expanded_line = process_heredoc_line(line, args);
	if (expanded_line)
	{
		write(args->fd, expanded_line, ft_strlen(expanded_line));
		write(args->fd, "\n", 1);
		free(expanded_line);
	}
	free(line);
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
	fprintf(stderr, "minishell: line 1: %s: %s\n", context, strerror(errno));
}
