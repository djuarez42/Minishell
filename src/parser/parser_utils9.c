/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils9.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 21:13:01 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/17 18:56:09 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_lines(char **lines, int count)
{
	while (count > 0)
		free(lines[--count]);
	free(lines);
}

static char	**init_heredoc_lines(int *capacity)
{
	char	**lines;

	*capacity = 1000;
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

static char	**collect_heredoc_loop(const char *delimiter,
			int interactive, char **lines, int *capacity)
{
	char	*line;
	int		count;

	count = 0;
	while (1)
	{
		line = read_heredoc_line(interactive);
		if (!line)
		{
			free_lines(lines, count);
			return (NULL);
		}
		if (is_delimiter(line, delimiter))
		{
			free(line);
			break ;
		}
		if (!store_heredoc_line(lines, line, &count, capacity))
			return (NULL);
	}
	lines[count] = NULL;
	return (lines);
}

char	**collect_heredoc_content(const char *delimiter,
		bool quoted __attribute__((unused)))
{
	char	**lines;
	int		capacity;
	int		interactive;

	interactive = isatty(STDIN_FILENO);
	lines = init_heredoc_lines(&capacity);
	if (!lines)
		return (NULL);
	return (collect_heredoc_loop(delimiter, interactive, lines, &capacity));
}
