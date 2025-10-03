/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_reader.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/03 13:26:51 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*g_heredoc_state[2];

void	heredoc_buffer_init(const char *content)
{
	if (g_heredoc_state[0])
		free(g_heredoc_state[0]);
	g_heredoc_state[0] = ft_strdup(content);
	g_heredoc_state[1] = g_heredoc_state[0];
}

void	heredoc_buffer_free(void)
{
	if (g_heredoc_state[0])
	{
		free(g_heredoc_state[0]);
		g_heredoc_state[0] = NULL;
	}
	g_heredoc_state[1] = NULL;
}

char	*heredoc_buffer_readline(void)
{
	char	*line;
	char	*start;
	char	*end;

	if (!g_heredoc_state[0] || !g_heredoc_state[1] || !(*g_heredoc_state[1]))
		return (NULL);
	start = g_heredoc_state[1];
	end = start;
	while (*end && *end != '\n')
		end++;
	line = ft_strndup(start, end - start);
	if (*end == '\n')
		g_heredoc_state[1] = end + 1;
	else
		g_heredoc_state[1] = end;
	return (line);
}

int	heredoc_buffer_active(void)
{
	return (g_heredoc_state[0] != NULL);
}
