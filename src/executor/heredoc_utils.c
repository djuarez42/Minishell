/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:15 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/09 23:04:22 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>
#include <string.h>

// Helper function to convert int to string (buffer-based to avoid allocation)
static void	ft_itoa_buffer(int n, char *buffer)
{
	int		i;
	int		len;
	long	nb;

	nb = n;
	len = (nb <= 0) ? 1 : 0;
	if (nb < 0)
		nb = -nb;
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
		buffer[i--] = nb % 10 + '0';
		nb /= 10;
	}
}

int	open_heredoc_file(t_heredoc_args *args)
{
	int			fd;
	static int	heredoc_counter = 0;
	char		count_str[16];
	char		*path;
	
	// Generate a unique filename using pid and counter
	ft_itoa_buffer(getpid(), count_str);
	path = ft_strjoin("/tmp/.heredoc_", count_str);
	ft_itoa_buffer(heredoc_counter++, count_str);
	path = ft_strjoin_free(path, "_");
	path = ft_strjoin_free(path, count_str);
	
	// Store the path for later cleanup
	args->heredoc_path = path;
	
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open heredoc");
		free(path);
		args->heredoc_path = NULL;
		return (-1);
	}
	return (fd);
}

int write_heredoc_lines(t_heredoc_args *args)
{
    char *line;
    char *expanded_line;
    int result = 0;
    void (*original_sigint)(int);
    void (*original_sigquit)(int);

    original_sigint = signal(SIGINT, SIG_DFL);
    original_sigquit = signal(SIGQUIT, SIG_IGN);
    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            result = 130;
            break;
        }


        if ((ft_strncmp(line, args->delimiter, ft_strlen(args->delimiter)) == 0 &&
             line[ft_strlen(args->delimiter)] == '\0'))
        {
            free(line);
            break;
        }

        if (!args->quoted)
            expanded_line = expand_variables(line, args->envp, args->state, QUOTE_NONE);
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
    return result;
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
		return;
	}
	close(fd);
}

void	cleanup_heredoc_file(t_heredoc_args *args)
{
	if (args->heredoc_path)
	{
		unlink(args->heredoc_path); // Delete the temporary file
		free(args->heredoc_path);
		args->heredoc_path = NULL;
	}
}

int	handle_redirections_heredoc(const char *delimiter, bool quoted,
			char **envp, t_heredoc_args *args)
{
	int	fd;
	int	res;

	// Initialize heredoc args
	args->delimiter = delimiter;
	args->quoted = quoted;
	args->envp = envp;
	args->heredoc_path = NULL;

	// Open unique heredoc file
	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);
	
	args->fd = fd;
	
	// Write heredoc content
	res = write_heredoc_lines(args);
	close(fd);
	
	if (res == 130)
		return (130); // SIGINT handling
	
	// Redirect stdin to read from heredoc file
	redirect_stdin_heredoc(args->heredoc_path);
	
	return (0);
}
