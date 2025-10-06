/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_helpers.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 19:40:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/06 13:30:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	expand_home_marker(const char *filename, char *expanded,
			const char **target)
{
	char	*home;

	*target = filename;
	if (filename[0] == '$' && strcmp(filename, "$HOME") == 0)
	{
		home = getenv("HOME");
		if (home)
		{
			ft_strlcpy(expanded, home, ft_strlen(home) + 1);
			*target = expanded;
		}
	}
	return (0);
}

int	check_target_is_dir(const char *target, t_exec_state *state)
{
	struct stat	st;

	if (stat(target, &st) == 0 && S_ISDIR(st.st_mode))
	{
		errno = EISDIR;
		print_error_file(target);
		if (state)
			state->last_status = 1;
		return (1);
	}
	return (0);
}

int	attempt_append_target(const char *target, t_exec_state *state)
{
	int	fd;

	fd = open(target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		print_error_file(target);
		if (state)
			state->last_status = 1;
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		print_error_file("dup2");
		close(fd);
		if (state)
			state->last_status = 1;
		return (1);
	}
	close(fd);
	return (0);
}

int	prepare_append_filename(const char *filename, char *expanded,
			const char **target, t_exec_state *state)
{
	if (!filename)
	{
		errno = EINVAL;
		print_error_file("null filename");
		if (state)
			state->last_status = 1;
		return (1);
	}
	*target = filename;
	if (expand_home_marker(filename, expanded, target))
		return (1);
	if (check_target_is_dir(*target, state))
		return (1);
	return (0);
}
