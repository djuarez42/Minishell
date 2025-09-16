/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils8.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 21:06:03 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 21:11:20 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	is_quoted_fragments(t_fragment *frag)
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

static char	*build_redir_file(t_fragment *frag)
{
	t_token	dummy_tok;
	char	*file;

	dummy_tok.fragments = frag;
	file = concat_final_text(&dummy_tok);
	if (!file)
		return (NULL);
	return (file);
}

static char	**build_heredoc_content(const char *file, bool quoted)
{
	char	**content;

	content = collect_heredoc_content(file, quoted);
	if (!content)
		return (NULL);
	return (content);
}

static t_redir	*init_redir(t_token *cur)
{
	t_redir		*redir;
	t_fragment	*frag;

	if (!cur || !cur->next || !cur->next->fragments)
		return (NULL);
	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = cur->type;
	redir->fragments = cur->next->fragments;
	frag = redir->fragments;
	redir->quoted = is_quoted_fragments(frag);
	redir->file = build_redir_file(frag);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->next = NULL;
	redir->heredoc_content = NULL;
	return (redir);
}

t_redir	*create_redir(t_token *cur)
{
	t_redir	*redir;

	redir = init_redir(cur);
	if (!redir)
		return (NULL);
	if (redir->type == TOKEN_HEREDOC)
	{
		redir->heredoc_content = build_heredoc_content(redir->file,
				redir->quoted);
		if (!redir->heredoc_content)
		{
			free(redir->file);
			free(redir);
			return (NULL);
		}
	}
	return (redir);
}
