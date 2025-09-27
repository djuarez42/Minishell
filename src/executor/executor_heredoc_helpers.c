/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_helpers.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 19:55:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:51:45 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_heredoc_delimiter(const char *cmp, const char *delim)
{
	size_t	dlen;

	dlen = ft_strlen(delim);
	if (ft_strncmp(cmp, delim, dlen) == 0 && cmp[dlen] == '\0')
		return (1);
	return (0);
}

int	write_line_to_heredoc_fd(int fd, const char *line, const char *expanded)
{
	if (expanded)
	{
		write(fd, expanded, ft_strlen(expanded));
		return (0);
	}
	write(fd, line, ft_strlen(line));
	return (0);
}

/*
** process_heredoc_line:
** Expand variables in heredoc lines when heredoc is unquoted.
** Returns a newly allocated string (expanded) or NULL on allocation failure.
*/
char	*process_heredoc_line(const char *line, t_heredoc_args *args)
{
	if (!args->quoted)
		return (expand_variables(line, args->envp, args->state, QUOTE_NONE));
	return (ft_strdup(line));
}

const char	*get_cmp_for_heredoc(char *line, char **expanded_line,
		t_heredoc_args *args)
{
	if (!args->quoted)
	{
		*expanded_line = process_heredoc_line(line, args);
		if (*expanded_line)
			return (*expanded_line);
		return (line);
	}
	*expanded_line = NULL;
	return (line);
}

int	handle_delimiter_and_cleanup(const char *cmp, const char *delim,
		char *line, char *expanded_line)
{
	if (is_heredoc_delimiter(cmp, delim))
	{
		free(line);
		if (expanded_line)
			free(expanded_line);
		return (0);
	}
	return (-1);
}
