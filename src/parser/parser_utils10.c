/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils10.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 21:36:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 22:13:44 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*read_heredoc_line(int interactive)
{
	char	*line;
	char	*buffer;
	size_t	len;
	ssize_t	read_len;

	if (interactive)
		return (readline("> "));
	buffer = NULL;
	len = 0;
	read_len = getline(&buffer, &len, stdin);
	if (read_len == -1)
	{
		free(buffer);
		return (NULL);
	}
	if (read_len > 0 && buffer[read_len - 1] == '\n')
		buffer[read_len - 1] = '\0';
	line = ft_strdup(buffer);
	free(buffer);
	return (line);
}

int	is_delimiter(const char *line, const char *delimiter)
{
	if (!line)
		return (0);
	if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
		&& line[ft_strlen(delimiter)] == '\0')
		return (1);
	return (0);
}

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
