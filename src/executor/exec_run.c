/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 22:46:32 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 22:46:32 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		return (2);
	}
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
		return (127);
	}
	return (execute_execve(exec_path, cmd->argv, envp));
}
