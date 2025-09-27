/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:42:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 20:07:54 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* helpers moved to executor_helpers.c */
int		run_pipeline(t_cmd *start, size_t n_cmds, char **envp,
			t_exec_state *state);

int		apply_redir_handle_bi(t_cmd *cur, char ***envp, t_exec_state *state);

int		run_parent_bi(t_cmd *cur, char ***envp, t_exec_state *state);

t_cmd	*hndle_emptargs(t_cmd *cur, char ***penvp, t_exec_state *state);

static t_cmd	*handle_cmd(t_cmd *cur, char ***penvp, t_exec_state *state)
{
	char	**envp;
	size_t	n;
	int		status;

	envp = *penvp;
	n = count_pipeline_cmds(cur);
	if (n == 1 && is_builtin(cur->argv[0]) && cur->pipe == 0)
		status = run_parent_bi(cur, penvp, state);
	else
		status = run_pipeline(cur, n, envp, state);
	state->last_status = status;
	while (n-- > 0 && cur)
		cur = cur->next;
	return (cur);
}

void	executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state)
{
	t_cmd	*cur;

	cur = cmd_list;
	while (cur)
	{
		if (!cur->argv || !cur->argv[0] || cur->argv[0][0] == '\0')
			cur = hndle_emptargs(cur, penvp, state);
		else
			cur = handle_cmd(cur, penvp, state);
	}
}
