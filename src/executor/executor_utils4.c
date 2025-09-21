/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:52:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 21:14:27 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc_redir(t_redir *redir)
{
	t_fragment	*frag;
	int			quoted;
	char		*tmp;

	frag = redir->fragments;
	quoted = 0;
	while (frag)
	{
		if (frag->quote_type == QUOTE_SINGLE
			|| frag->quote_type == QUOTE_DOUBLE)
		{
			quoted = 1;
			break ;
		}
		frag = frag->next;
	}
	redir->quoted = quoted;
	tmp = build_heredoc_delimiter(redir->file);
	if (!tmp)
		return (1);
	free(redir->file);
	redir->file = tmp;
	return (0);
}

static void	process_unquoted_filename(t_redir *redir)
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

	redir = redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			if (process_heredoc_redir(redir) != 0)
				return (1);
		}
		else
			process_unquoted_filename(redir);
		redir = redir->next;
	}
	return (handle_redirections(redirs, envp, state));
}

/* execute_command, find_executable, execute_execve and str_append are
** implemented in executor_utils5.c */

char	*build_heredoc_delimiter(const char *text)
{
	t_fragment	*fragments;
	char		*delimiter;
	t_token		dummy_tok;

	if (!text)
		return (ft_strdup(""));
	fragments = parse_mixed_fragments(text);
	if (!fragments)
		return (ft_strdup(text));
	dummy_tok.fragments = fragments;
	delimiter = concat_final_text(&dummy_tok);
	free_fragments(fragments);
	return (delimiter);
}
