/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_error_format.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:21:32 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/26 17:58:35 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_error(const char *context, const char *message)
{
	ft_putstr_fd("minishell: ", 2);
	if (!isatty(STDIN_FILENO))
	{
		ft_putstr_fd("line 1: ", 2);
	}
	if (context && *context)
	{
		ft_putstr_fd((char *)context, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd((char *)message, 2);
}

void	print_execve_error(const char *filename)
{
	ft_putstr_fd("minishell: ", 2);
	if (!isatty(STDIN_FILENO))
		ft_putstr_fd("line 1: ", 2);
	ft_putstr_fd((char *)filename, 2);
	ft_putstr_fd(": ", 2);
	if (errno == ENOENT)
		ft_putendl_fd("command not found", 2);
	else if (errno == EACCES)
		ft_putendl_fd("Permission denied", 2);
	else if (errno == EISDIR)
		ft_putendl_fd("Is a directory", 2);
	else
		ft_putendl_fd(strerror(errno), 2);
}
