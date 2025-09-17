/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:21:10 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/17 17:17:55 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "libft.h"
#include <stdio.h>
#include <string.h>

static const char	*get_base_cmd(const char *argv0)
{
	const char	*slash;

	slash = ft_strrchr(argv0, '/');
	if (slash)
		return (slash + 1);
	return (argv0);
}

int	is_builtin(const char *cmd)
{
	const char	*base;

	if (!cmd)
		return (0);
	base = get_base_cmd(cmd);
	return (!ft_strncmp(base, "echo", 5)
		|| !ft_strncmp(base, "cd", 3)
		|| !ft_strncmp(base, "pwd", 4)
		|| !ft_strncmp(base, "export", 7)
		|| !ft_strncmp(base, "unset", 6)
		|| !ft_strncmp(base, "env", 4)
		|| !ft_strncmp(base, "exit", 5));
}

static int	dispatch_builtin(t_cmd *cmd, char ***penvp)
{
	const char	*base_cmd;

	base_cmd = get_base_cmd(cmd->argv[0]);
	if (!ft_strncmp(base_cmd, "echo", 5))
		return (bi_echo(cmd));
	if (!ft_strncmp(base_cmd, "pwd", 4))
		return (bi_pwd());
	if (!ft_strncmp(base_cmd, "env", 4))
		return (bi_env(cmd->argv, *penvp));
	if (!ft_strncmp(base_cmd, "cd", 3))
		return (bi_cd(cmd->argv, penvp));
	if (!ft_strncmp(base_cmd, "export", 7))
		return (bi_export(cmd->argv, penvp));
	if (!ft_strncmp(base_cmd, "unset", 6))
		return (bi_unset(cmd->argv, penvp));
	if (!ft_strncmp(base_cmd, "exit", 5))
		return (bi_exit(cmd->argv));
	return (1);
}

int	run_builtin_in_child(t_cmd *cmd, char ***penvp)
{
	return (dispatch_builtin(cmd, penvp));
}

int	run_builtin_in_parent(t_cmd *cmd, char ***penvp)
{
	int	status;

	status = dispatch_builtin(cmd, penvp);
	if (!ft_strncmp(get_base_cmd(cmd->argv[0]), "exit", 5))
		exit(status);
	return (status);
}
