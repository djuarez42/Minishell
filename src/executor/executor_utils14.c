/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils14.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 17:26:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 04:13:04 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	itoa_buffer_int(int n, char *buffer)
{
	int		len;
	int		i;
	unsigned int	nb;

	len = (n <= 0) ? 1 : 0;
	nb = (n < 0) ? -n : n;
	i = nb;
	while (i > 0)
	{
		len++;
		i /= 10;
	}
	buffer[len] = '\0';
	if (n < 0)
		buffer[0] = '-';
	else if (n == 0)
		buffer[0] = '0';
	i = len - 1;
	while (nb > 0)
	{
		buffer[i--] = (nb % 10) + '0';
		nb /= 10;
	}
}

int	open_heredoc_file(t_heredoc_args *args)
{
	int		fd;
	char		*path;
	static int	counter = 0;
	char		count_str[16];

	itoa_buffer_int(getpid(), count_str);
	path = ft_strjoin("/tmp/.heredoc_", count_str);
	itoa_buffer_int(counter++, count_str);
	path = ft_strjoin_free(path, "_");
	path = ft_strjoin_free(path, count_str);
	args->heredoc_path = path;
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open heredoc");
		args->heredoc_path = NULL;
		return (-1);
	}
	return (fd);
}

int	write_heredoc_lines(t_heredoc_args *args)
{
	char	*line;
	char	*expanded_line;
	int		result;
	void	(*original_sigint)(int);
	void	(*original_sigquit)(int);

	result = 0;
	original_sigint = signal(SIGINT, SIG_DFL);
	original_sigquit = signal(SIGQUIT, SIG_IGN);

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			result = 130;
			break ;
		}
		if ((ft_strncmp(line, args->delimiter,
					ft_strlen(args->delimiter)) == 0
				&& line[ft_strlen(args->delimiter)] == '\0'))
		{
			free(line);
			break ;
		}
		if (!args->quoted)
			expanded_line = expand_variables(line, args->envp,
					args->state, QUOTE_NONE);
		else
			expanded_line = ft_strdup(line);

		if (expanded_line)
		{
			write(args->fd, expanded_line, ft_strlen(expanded_line));
			write(args->fd, "\n", 1);
			free(expanded_line);
		}
		free(line);
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
