/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils5.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:56:44 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 21:36:18 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc(t_redir *redir)
{
	t_fragment	*frag;
	bool		is_quoted;
	char		*built_delim;

	frag = redir->fragments;
	is_quoted = false;
	while (frag)
	{
		if (frag->quote_type == QUOTE_SINGLE
			|| frag->quote_type == QUOTE_DOUBLE)
		{
			is_quoted = true;
			break ;
		}
		frag = frag->next;
	}
	redir->quoted = is_quoted;
	built_delim = build_heredoc_delimiter(redir->file);
	if (!built_delim)
		return (1);
	free(redir->file);
	redir->file = built_delim;
	return (0);
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
			res = process_heredoc(redir);
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
