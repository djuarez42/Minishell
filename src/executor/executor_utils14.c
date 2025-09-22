/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils14.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 17:26:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 04:37:48 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	compute_int_len(int n)
{
	int				len;
	unsigned int	nb;
	unsigned int	tmp;

	len = 0;
	if (n <= 0)
		len = 1;
	if (n < 0)
		nb = -n;
	else
		nb = n;
	tmp = nb;
	while (tmp > 0)
	{
		len++;
		tmp /= 10;
	}
	return (len);
}

static void	fill_number_buffer(unsigned int nb, char *buffer, int len)
{
	int	i;

	i = len - 1;
	while (nb > 0)
	{
		buffer[i] = (nb % 10) + '0';
		nb /= 10;
		i--;
	}
}

static void	itoa_buffer_int(int n, char *buffer)
{
	int				len;
	unsigned int	nb;

	len = compute_int_len(n);
	buffer[len] = '\0';
	if (n < 0)
	{
		buffer[0] = '-';
		nb = -n;
	}
	else if (n == 0)
	{
		buffer[0] = '0';
		nb = 0;
	}
	else
		nb = n;
	if (nb > 0)
		fill_number_buffer(nb, buffer, len);
}

/*static void	itoa_buffer_int(int n, char *buffer)
{
	int				len;
	int				i;
	unsigned int	nb;

	len = 0;
	if (n <= 0)
		len = 1;
	if (n < 0)
		nb = -n;
	else
		nb = n;
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
		buffer[i] = (nb % 10) + '0';
		nb /= 10;
		i--;
	}
}*/

/*static void	itoa_buffer_int(int n, char *buffer)
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
}*/

int	open_heredoc_file(t_heredoc_args *args)
{
	int			fd;
	char		*path;
	static int	counter;
	char		count_str[16];

	counter = 0;
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

/*int	open_heredoc_file(t_heredoc_args *args)
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
}*/

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
