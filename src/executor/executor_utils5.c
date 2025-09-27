/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils5.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:56:44 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 20:00:08 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc(t_redir *redir, char **envp, t_exec_state *state)
{
	return (prepare_heredoc_delim(redir, envp, state));
}

static void	process_remove_quotes(t_redir *redir)
{
	char	*tmp;

	tmp = remove_quotes(redir->file);
	free(redir->file);
	redir->file = tmp;
}

int	handle_redirections_and_quotes(t_redir *redirs, char **envp,
		t_exec_state *state)
{
	t_redir	*redir;
	int		res;

	redir = redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			res = process_heredoc(redir, envp, state);
			if (res != 0)
				return (1);
		}
		else
			process_remove_quotes(redir);
		redir = redir->next;
	}
	res = handle_redirections(redirs, envp, state);
	return (res);
}
