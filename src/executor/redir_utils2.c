/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 17:26:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 14:06:54 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	open_heredoc_file(void)
{
	int	fd;

	fd = open("/tmp/.heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open heredoc");
		exit(1);
	}
	return (fd);
}

int	write_heredoc_lines(t_heredoc_args *args)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			return (130);
		}
		if ((ft_strncmp(line, args->delimiter,
					ft_strlen(args->delimiter)) == 0
				&& line[ft_strlen(args->delimiter)] == '\0'))
		{
			free(line);
			break ;
		}
		if (!args->quoted)
			expanded_line = expand_variables(line, args->envp, args->state,
					QUOTE_NONE);
		else
			expanded_line = ft_strdup(line);
		write(args->fd, expanded_line, ft_strlen(expanded_line));
		write(args->fd, "\n", 1);
		free(expanded_line);
		free(line);
	}
	return (0);
}

void	redirect_stdin_heredoc(void)
{
	int	fd;

	fd = open("/tmp/.heredoc_tmp", O_RDONLY);
	if (fd == -1 || dup2(fd, STDIN_FILENO) == -1)
	{
		perror("heredoc redirect");
		if (fd != -1)
			close(fd);
		exit(1);
	}
	close(fd);
}

void print_error_file(const char *context)
{
    fprintf(stderr, "minishell: line 1: %s: %s\n", context, strerror(errno));
}