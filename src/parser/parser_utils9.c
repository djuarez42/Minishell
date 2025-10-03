/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils9.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 21:13:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 16:59:24 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**init_heredoc_lines(int *capacity)
{
	char	**lines;

	*capacity = 666;
	lines = malloc(sizeof(char *) * (*capacity));
	if (!lines)
		return (NULL);
	return (lines);
}

static int	store_heredoc_line(char **lines, char *line, int *count,
		int *capacity)
{
	if (*count >= *capacity - 1)
	{
		lines = expand_lines_array(lines, capacity, *count);
		if (!lines)
		{
			free(line);
			return (0);
		}
	}
	lines[*count] = line;
	(*count)++;
	return (1);
}

static void	print_heredoc_warning(const char *delimiter)
{
	ft_putstr_fd("warning: here-document at line 1 delimited by ",
		STDERR_FILENO);
	ft_putstr_fd("end-of-file (wanted `", STDERR_FILENO);
	if (delimiter)
		ft_putstr_fd((char *)delimiter, STDERR_FILENO);
	ft_putendl_fd("')", STDERR_FILENO);
}

static int	handle_heredoc_line(char *line, const char *delim)
{
	if (is_delimiter(line, delim))
	{
		free(line);
		return (0);
	}
	return (1);
}

char	**collect_heredoc_content(const char *delimiter,
		bool quoted __attribute__((unused)), t_exec_state *state)
{
	char	**lines;
	char	*line;
	int		vars[3];

	vars[1] = isatty(STDIN_FILENO);
	lines = init_heredoc_lines(&vars[0]);
	if (!lines)
		return (NULL);
	vars[2] = 0;
	while (1)
	{
		line = read_heredoc_line(vars[1], state);
		if (!line && (print_heredoc_warning(delimiter), 1))
			break ;
		if (!handle_heredoc_line(line, delimiter))
			break ;
		if (!store_heredoc_line(lines, line, &vars[2], &vars[0]))
			return (NULL);
	}
	lines[vars[2]] = NULL;
	return (lines);
}
