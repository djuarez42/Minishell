/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_collection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 13:51:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/03 13:57:43 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*append_line_to_buffer(char *buffer, char *line)
{
	buffer = ft_strjoin_free(buffer, line);
	if (!buffer)
		return (NULL);
	buffer = ft_strjoin_free(buffer, "\n");
	return (buffer);
}

static char	*read_until_delimiter(const char *delimiter, char *buffer)
{
	char	*line;

	while (1)
	{
		line = get_next_line(STDIN_FILENO);
		if (!line)
			break ;
		buffer = append_line_to_buffer(buffer, line);
		free(line);
		if (!buffer || ft_strcmp(buffer + ft_strlen(buffer)
				- ft_strlen(delimiter) - 1, delimiter) == 0)
			break ;
	}
	return (buffer);
}

char	*collect_heredoc_bodies_into_buffer(char **delimiters)
{
	char	*buffer;
	int		i;

	if (!delimiters || !delimiters[0])
		return (NULL);
	buffer = ft_strdup("");
	if (!buffer)
		return (NULL);
	i = 0;
	while (delimiters[i])
	{
		buffer = read_until_delimiter(delimiters[i], buffer);
		if (!buffer)
			return (NULL);
		i++;
	}
	return (buffer);
}
