/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_reader.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 12:30:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/03 16:59:05 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_buffer_init(t_exec_state *state, const char *content)
{
	if (state->heredoc_buffer)
		free(state->heredoc_buffer);
	state->heredoc_buffer = ft_strdup(content);
	state->heredoc_position = state->heredoc_buffer;
}

void	heredoc_buffer_free(t_exec_state *state)
{
	if (state->heredoc_buffer)
	{
		free(state->heredoc_buffer);
		state->heredoc_buffer = NULL;
	}
	state->heredoc_position = NULL;
}

char	*heredoc_buffer_readline(t_exec_state *state)
{
	char	*line;
	char	*start;
	char	*end;

	if (!state->heredoc_buffer || !state->heredoc_position
		|| !(*state->heredoc_position))
		return (NULL);
	start = state->heredoc_position;
	end = start;
	while (*end && *end != '\n')
		end++;
	line = ft_strndup(start, end - start);
	if (*end == '\n')
		state->heredoc_position = end + 1;
	else
		state->heredoc_position = end;
	return (line);
}

int	heredoc_buffer_active(t_exec_state *state)
{
	return (state->heredoc_buffer != NULL);
}
