/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils10.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 21:36:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 14:17:01 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**expand_lines_array(char **lines, int *capacity, int count)
{
	char	**new_lines;
	int		i;

	*capacity *= 2;
	new_lines = malloc(sizeof(char *) * (*capacity));
	if (!new_lines)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_lines[i] = lines[i];
		i++;
	}
	free(lines);
	return (new_lines);
}

static char	*create_new_str(char c)
{
	char	*new_str;

	new_str = malloc(2);
	if (!new_str)
		return (NULL);
	new_str[0] = c;
	new_str[1] = '\0';
	return (new_str);
}

static char	*append_char_existing(char *s, char c)
{
	char	*new_str;
	size_t	i;
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	new_str = malloc(len + 2);
	if (!new_str)
	{
		free(s);
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		new_str[i] = s[i];
		i++;
	}
	new_str[i] = c;
	new_str[i + 1] = '\0';
	free(s);
	return (new_str);
}

char	*str_append_char(char *s, char c)
{
	if (!s)
		return (create_new_str(c));
	return (append_char_existing(s, c));
}

char	*read_heredoc_line(int interactive)
{
	char	c;
	char	*line;
	ssize_t	ret;

	line = NULL;
	ret = 0;
	if (interactive)
		return (readline("> "));
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0 || c == '\n')
			break ;
		line = str_append_char(line, c);
		if (!line)
			return (NULL);
	}
	if (ret == -1)
	{
		free(line);
		return (NULL);
	}
	return (line);
}
