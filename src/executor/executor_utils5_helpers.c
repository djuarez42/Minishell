/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils5_helpers.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 21:04:38 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 21:16:15 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	frag_list_has_quotes(t_fragment *frag);

int	prepare_heredoc_delim(t_redir *redir, char **envp, t_exec_state *state)
{
	t_fragment	*frag;
	bool		is_quoted;
	char		*built_delim;

	frag = redir->fragments;
	is_quoted = frag_list_has_quotes(frag);
	redir->quoted = is_quoted;
	if (redir->quoted)
		built_delim = remove_quotes(redir->file);
	else
		built_delim = expand_variables(
				redir->file, envp, state, QUOTE_NONE);
	if (!built_delim)
		return (1);
	free(redir->file);
	redir->file = built_delim;
	return (0);
}

static bool	frag_list_has_quotes(t_fragment *frag)
{
	while (frag)
	{
		if (frag->quote_type == QUOTE_SINGLE
			|| frag->quote_type == QUOTE_DOUBLE)
			return (true);
		frag = frag->next;
	}
	return (false);
}
