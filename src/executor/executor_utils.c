/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:34:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/20 16:28:05 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

void	free_split(char **split)
{
	int	i;

	i = 0;
	while (split && split[i])
		free(split[i++]);
	free(split);
}

int	handle_redirections_and_quotes(t_redir *redirs, char **envp)
{
	t_redir	*redir;
	int		res;

	redir = redirs;
	while (redir)
	{
		redir->file = remove_quotes(redir->file);
		redir = redir->next;
	}
	res = handle_redirections(redirs, envp);
	return (res);
}

void	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		ft_putstr_fd("bash: line 1: ", 2);
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	execute_execve(exec_path, cmd->argv, envp);
}
