/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 21:21:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 19:52:52 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	store_final_text(t_token *tok, t_proc_ctx *ctx)
{
	char	*dup;

	if (ctx->cmd->argv_final_text)
	{
		if (tok->final_text)
			dup = ft_strdup(tok->final_text);
		else
			dup = ft_strdup("");
		ctx->cmd->argv_final_text[*ctx->argc_final_text] = dup;
		(*ctx->argc_final_text)++;
	}
}

static int	process_assignment_case(t_token *tok, t_proc_ctx *ctx, int idx)
{
	char	*dup;

	if (tok->final_text)
		dup = ft_strdup(tok->final_text);
	else
		dup = ft_strdup("");
	ctx->cmd->argv[idx] = dup;
	ctx->cmd->argv_quote[idx] = detect_combined_quote(tok->fragments);
	idx++;
	return (idx);
}

static int	process_words_case(t_token *tok, t_proc_ctx *ctx, int idx)
{
	char	**words;
	int		nwords;
	int		w;

	nwords = 0;
	words = build_words_from_token(tok, &nwords);
	if (words)
	{
		w = 0;
		while (w < nwords)
		{
			ctx->cmd->argv[idx] = ft_strdup(words[w]);
			ctx->cmd->argv_quote[idx] = detect_combined_quote(tok->fragments);
			idx++;
			w++;
		}
		free_str_array(words);
	}
	return (idx);
}

static int	process_assignment_or_words(t_token *tok, t_proc_ctx *ctx, int idx)
{
	t_fragment	*frag;
	char		*text;

	frag = tok->fragments;
	text = NULL;
	if (frag && frag->text)
		text = frag->text;
	if (frag && ft_strchr(text, '='))
		idx = process_assignment_case(tok, ctx, idx);
	else
		idx = process_words_case(tok, ctx, idx);
	return (idx);
}

char	**process_token_with_quotes(t_token *tok, t_proc_ctx *ctx)
{
	int	idx;

	if (!tok || !ctx || !ctx->cmd)
		return (NULL);
	idx = *ctx->argc_argv;
	update_final_text(tok, ctx);
	store_final_text(tok, ctx);
	idx = process_assignment_or_words(tok, ctx, idx);
	*ctx->argc_argv = idx;
	return (ctx->cmd->argv);
}
