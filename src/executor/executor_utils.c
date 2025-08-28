/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:34:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/28 19:55:44 by ekakhmad         ###   ########.fr       */
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

int	handle_redirections_and_quotes(t_redir *redirs, char **envp, t_exec_state *state)
{
	int		res;

	// Expand redirections here (following flowchart order)
	if (expand_redirs(redirs, envp, state) == -1)
		return (-1);
	
	res = handle_redirections(redirs, envp, state);
	return (res);
}

int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		return (2);
	}
	
	// Remove debug code for production
	/*
	fprintf(stderr, "DEBUG: Command: [%s]\n", cmd->argv[0]);
	for (int i = 1; cmd->argv[i] != NULL; i++) {
		fprintf(stderr, "DEBUG: Arg %d: [%s]\n", i, cmd->argv[i]);
	}
	*/
	
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
		return (127);
	}
	// fprintf(stderr, "DEBUG: Executing with path: [%s]\n", exec_path);
	return (execute_execve(exec_path, cmd->argv, envp));
}
