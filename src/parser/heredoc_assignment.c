/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_assignment.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/03 13:25:16 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**expand_lines_array_heredoc(char **lines, int count, int *capacity)
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

static int	store_heredoc_line(char **lines, int *count,
				int *capacity, char *line)
{
	if (*count >= *capacity - 1)
	{
		lines = expand_lines_array_heredoc(lines, *count, capacity);
		if (!lines)
			return (0);
	}
	lines[(*count)++] = line;
	return (1);
}

static char	**split_heredoc_body(const char *delimiter)
{
	char	*line;
	char	**lines;
	int		count;
	int		capacity;

	capacity = 8;
	lines = malloc(sizeof(char *) * capacity);
	if (!lines)
		return (NULL);
	count = 0;
	while (1)
	{
		line = heredoc_buffer_readline();
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		if (!store_heredoc_line(lines, &count, &capacity, line))
			return (NULL);
	}
	lines[count] = NULL;
	return (lines);
}

static void	assign_heredoc_to_redir(t_redir *redir)
{
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC && heredoc_buffer_active())
			redir->heredoc_content = split_heredoc_body(redir->file);
		redir = redir->next;
	}
}

void	assign_heredocs_from_buffer(t_cmd *cmd_list)
{
	t_cmd	*cmd;

	if (!heredoc_buffer_active())
		return ;
	cmd = cmd_list;
	while (cmd)
	{
		if (cmd->redirs)
			assign_heredoc_to_redir(cmd->redirs);
		cmd = cmd->next;
	}
}
