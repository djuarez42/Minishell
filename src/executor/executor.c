/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:42:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 16:30:08 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	run_pipeline(t_cmd *start, size_t n_cmds, char **envp,
		t_exec_state *state)
{
	(void)start;
	(void)n_cmds;
	(void)envp;
	(void)state;
	return (fork_and_run_pipeline(start, n_cmds, envp, state));
}

static int	handle_empty_command(t_exec_state *state)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
		STDERR_FILENO);
	state->last_status = 2;
	return (0);
}

static int	apply_redirections_and_run_builtin(t_cmd *cur, char ***envp,
		t_exec_state *state)
{
	int	res;
	int	status;

	res = handle_redirections_and_quotes(cur->redirs, *envp, state);
	if (res == 130)
		status = 130;
	else if (res == 1)
		status = 1;
	else
		status = run_builtin_in_parent(cur, envp);
	return (status);
}

static int	run_parent_builtin(t_cmd *cur, char ***envp, t_exec_state *state)
{
	int	save_in;
	int	save_out;
	int	save_err;
	int	status;

	save_in = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	save_err = dup(STDERR_FILENO);
	status = apply_redirections_and_run_builtin(cur, envp, state);
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

void	executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)
{
	t_cmd	*cur;
	size_t	n;
	int		status;
	char	**envp;

	envp = *penvp;
	cur = cmd_list;
	while (cur)
	{
		if (!cur->argv || !cur->argv[0])
		{
			handle_empty_command(state);
			cur = cur->next;
			continue ;
		}
		n = count_pipeline_cmds(cur);
		if (n == 1 && is_builtin(cur->argv[0]) && cur->pipe == 0)
			status = run_parent_builtin(cur, &envp, state);
		else
			status = run_pipeline(cur, n, envp, state);
		state->last_status = status;
		while (n-- > 0 && cur)
			cur = cur->next;
	}
	*penvp = envp;
}
