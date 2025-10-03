/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 14:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/03 13:57:43 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define BUFFER_SIZE 1024

static char	*join_and_free(char *s1, char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

static int	check_newline(char *buffer, char **line)
{
	int		i;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
	{
		buffer[i] = '\0';
		*line = join_and_free(*line, buffer);
		ft_memmove(buffer, buffer + i + 1, ft_strlen(buffer + i + 1) + 1);
		return (1);
	}
	return (0);
}

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE + 1];
	char		*line;
	int			bytes_read;

	line = ft_strdup("");
	if (!line)
		return (NULL);
	while (1)
	{
		if (check_newline(buffer, &line))
			return (line);
		line = join_and_free(line, buffer);
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read <= 0)
			break ;
		buffer[bytes_read] = '\0';
	}
	if (bytes_read < 0 || (bytes_read == 0 && line[0] == '\0'))
	{
		free(line);
		return (NULL);
	}
	return (line);
}
