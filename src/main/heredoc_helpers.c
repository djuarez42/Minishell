/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 13:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/03 14:06:31 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_heredocs(const char *line)
{
	int		count;
	int		i;
	char	in_quote;

	count = 0;
	i = 0;
	in_quote = 0;
	while (line[i])
	{
		if (!in_quote && (line[i] == '\'' || line[i] == '"'))
			in_quote = line[i];
		else if (in_quote && line[i] == in_quote)
			in_quote = 0;
		else if (!in_quote && line[i] == '<' && line[i + 1] == '<'
			&& line[i + 2] != '<')
		{
			count++;
			i++;
		}
		i++;
	}
	return (count);
}

char	*extract_delimiter(const char *line, int *pos)
{
	int		start;
	int		end;
	char	quote;

	while (line[*pos] && (line[*pos] == ' ' || line[*pos] == '\t'))
		(*pos)++;
	start = *pos;
	if (line[*pos] == '\'' || line[*pos] == '"')
	{
		quote = line[(*pos)++];
		start = *pos;
		while (line[*pos] && line[*pos] != quote)
			(*pos)++;
		end = *pos;
		if (line[*pos] == quote)
			(*pos)++;
	}
	else
	{
		while (line[*pos] && line[*pos] != ' ' && line[*pos] != '\t'
			&& line[*pos] != '<' && line[*pos] != '>' && line[*pos] != '|')
			(*pos)++;
		end = *pos;
	}
	return (ft_strndup(line + start, end - start));
}

static char	**init_delims_array(int count)
{
	char	**delims;

	if (count == 0)
		return (NULL);
	delims = malloc(sizeof(char *) * (count + 1));
	return (delims);
}

static void	process_heredoc_in_line(const char *cmd_line, int *i,
		char **delims, int *idx)
{
	*i += 2;
	delims[(*idx)++] = extract_delimiter(cmd_line, i);
}

char	**find_heredoc_delimiters(const char *cmd_line)
{
	char	**delims;
	int		i;
	int		idx;
	char	in_quote;

	delims = init_delims_array(count_heredocs(cmd_line));
	if (!delims)
		return (NULL);
	i = -1;
	idx = 0;
	in_quote = 0;
	while (cmd_line[++i])
	{
		if ((!in_quote && (cmd_line[i] == '\'' || cmd_line[i] == '"')))
			in_quote = cmd_line[i];
		else if (in_quote && cmd_line[i] == in_quote)
			in_quote = 0;
		else if (!in_quote && cmd_line[i] == '<' && cmd_line[i + 1] == '<'
			&& cmd_line[i + 2] != '<')
			process_heredoc_in_line(cmd_line, &i, delims, &idx);
	}
	delims[idx] = NULL;
	return (delims);
}
