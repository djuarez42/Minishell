/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 20:30:00 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/09/27 20:30:00 by ekakhmad        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_pipeline(t_cmd *start, size_t n_cmds, char **envp, t_exec_state *state)
{
	(void)start;
	(void)n_cmds;
	(void)envp;
	(void)state;
	return (fork_and_run_pipeline(start, n_cmds, envp, state));
}

int	apply_redir_handle_bi(t_cmd *cur, char ***envp, t_exec_state *state)
{
	int	res;
	int	status;

	res = handle_redirections_and_quotes(cur->redirs, *envp, state);
	if (res == 130)
		status = 130;
	else if (res == 1)
		status = 1;
	else
		status = run_bi_in_parent(cur, envp);
	return (status);
}

int	run_parent_bi(t_cmd *cur, char ***envp, t_exec_state *state)
{
	int	save_in;
	int	save_out;
	int	save_err;
	int	status;

	save_in = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	save_err = dup(STDERR_FILENO);
	status = apply_redir_handle_bi(cur, envp, state);
	if (save_in != -1)
		dup2(save_in, STDIN_FILENO);
	if (save_out != -1)
		dup2(save_out, STDOUT_FILENO);
	if (save_err != -1)
		dup2(save_err, STDERR_FILENO);
	if (save_in != -1)
		close(save_in);
	if (save_out != -1)
		close(save_out);
	if (save_err != -1)
		close(save_err);
	return (status);
}

t_cmd	*hndle_emptargs(t_cmd *cur, char ***penvp, t_exec_state *state)
{
	char	**envp;
	int		res;
	int		status;

	envp = *penvp;
	if (cur->redirs)
	{
		res = handle_redirections_and_quotes(cur->redirs, envp, state);
		if (res == 130 || res == 1)
			status = res;
		else
			status = 0;
		state->last_status = status;
		return (cur->next);
	}
	if (cur->pipe)
	{
		handle_empty_command(state);
		return (cur->next);
	}
	print_execve_error("");
	state->last_status = 127;
	return (cur->next);
}

int	handle_empty_command(t_exec_state *state)
{
	print_error(NULL, "syntax error near unexpected token `|' ");
	state->last_status = 2;
	return (0);
}
