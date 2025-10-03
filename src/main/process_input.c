/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 17:42:00 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 17:02:04 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (signo == SIGQUIT)
		(void)signo;
}

static bool	has_valid_command(t_cmd *cmds)
{
	t_cmd	*tmp_cmd;

	tmp_cmd = cmds;
	while (tmp_cmd)
	{
		if (tmp_cmd->argv && tmp_cmd->argv[0] && tmp_cmd->argv[0][0] != '\0')
			return (true);
		tmp_cmd = tmp_cmd->next;
	}
	return (false);
}

static void	execute_or_redir(t_cmd *cmds, char ***envp_copy,
		t_exec_state *state)
{
	if (has_valid_command(cmds))
		executor(cmds, envp_copy, state);
	else if (cmds->redirs)
		(void)handle_redirections(cmds->redirs, *envp_copy, state);
}

void	process_input(char *input, char ***envp_copy, t_exec_state *state)
{
	t_token	*tokens;
	t_cmd	*cmds;

	tokens = tokenize_input(input);
	if (!tokens)
		return ;
	cmds = parser_tokens(tokens, *envp_copy, state);
	if (cmds)
	{
		execute_or_redir(cmds, envp_copy, state);
		free_cmds(cmds);
	}
	free_token_list(tokens);
}
