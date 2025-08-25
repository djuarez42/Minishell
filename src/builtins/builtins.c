/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:21:10 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/23 20:21:11 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*   This file now contains only builtin dispatch to satisfy Norminette.      */
/*   Individual builtin implementations are split into dedicated files.       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "libft.h"

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	return (!ft_strncmp(cmd, "echo", 5)
		|| !ft_strncmp(cmd, "cd", 3)
		|| !ft_strncmp(cmd, "pwd", 4)
		|| !ft_strncmp(cmd, "export", 7)
		|| !ft_strncmp(cmd, "unset", 6)
		|| !ft_strncmp(cmd, "env", 4)
		|| !ft_strncmp(cmd, "exit", 5));
}

static int	dispatch_builtin(char **argv, char ***penvp)
{
	if (!ft_strncmp(argv[0], "echo", 5))
		return (bi_echo(argv));
	if (!ft_strncmp(argv[0], "pwd", 4))
		return (bi_pwd());
	if (!ft_strncmp(argv[0], "env", 4))
		return (bi_env(*penvp));
	if (!ft_strncmp(argv[0], "cd", 3))
		return (bi_cd(argv, penvp));
	if (!ft_strncmp(argv[0], "export", 7))
		return (bi_export(argv, penvp));
	if (!ft_strncmp(argv[0], "unset", 6))
		return (bi_unset(argv, penvp));
	if (!ft_strncmp(argv[0], "exit", 5))
		return (bi_exit(argv));
	return (1);
}

int	run_builtin_in_child(t_cmd *cmd, char ***penvp)
{
	return (dispatch_builtin(cmd->argv, penvp));
}

int	run_builtin_in_parent(t_cmd *cmd, char ***penvp)
{
	int	status;

	status = dispatch_builtin(cmd->argv, penvp);
	if (!ft_strncmp(cmd->argv[0], "exit", 5))
		exit(status);
	return (status);
}
