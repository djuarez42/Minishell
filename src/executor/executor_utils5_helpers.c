/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils5_helpers.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 20:10:00 by ekakhmad         #+#    #+#             */
/*   Updated: 2025/09/27 20:10:00 by ekakhmad        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	prepare_heredoc_delim(t_redir *redir, char **envp, t_exec_state *state)
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
	if (redir->quoted)
		built_delim = remove_quotes(redir->file);
	else
		built_delim = expand_variables(redir->file, envp, state, QUOTE_NONE);
	if (!built_delim)
		return (1);
	free(redir->file);
	redir->file = built_delim;
	return (0);
}
