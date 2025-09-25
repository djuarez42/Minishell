/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 02:39:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/25 20:53:12 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	get_next_word_range(t_word_builder *wb, size_t start,
				size_t *wlen)
{
	size_t	i;

	i = start;
	while (i < wb->len && ft_isspace((unsigned char)wb->buf[i])
		&& wb->splittable[i])
		i++;
	if (i >= wb->len)
	{
		*wlen = 0;
		return (i);
	}
	start = i;
	while (i < wb->len && !(ft_isspace((unsigned char)wb->buf[i])
			&& wb->splittable[i]))
		i++;
	*wlen = i - start;
	return (start);
}

char	**split_buffer_to_words(t_word_builder *wb)
{
	char		**words;
	size_t		i;
	int			wi;
	size_t		start;
	size_t		wlen;

	i = 0;
	wi = 0;
	words = malloc(sizeof(char *) * (wb->count + 1));
	if (!words)
		return (NULL);
	while (i < wb->len && wi < wb->count)
	{
		start = get_next_word_range(wb, i, &wlen);
		if (wlen == 0)
			break ;
		words[wi] = ft_strndup(&wb->buf[start], wlen);
		wi++;
		i = start + wlen;
	}
	words[wi] = NULL;
	return (words);
}

char	**build_words_from_buffer(t_fragment *frag,
										t_word_builder *wb,
										int *out_count)
{
	char	**words;

	fill_buffer_and_splittable(frag, wb);
	count_words(wb);
	if (wb->count == 0)
	{
		words = build_from_quoted_fragments(frag, out_count);
		return (words);
	}
	words = split_buffer_to_words(wb);
	return (words);
}

char	**build_words_from_token(t_token *tok, int *out_count)
{
	t_fragment		*frag;
	t_word_builder	wb;
	char			**words;

	if (!tok || !out_count)
		return (NULL);
	frag = tok->fragments;
	calc_total_len(frag, &wb);
	wb.buf = malloc(wb.total_len + 1);
	wb.splittable = malloc(wb.total_len);
	if (!wb.buf || !wb.splittable)
	{
		free(wb.buf);
		free(wb.splittable);
		*out_count = 0;
		return (NULL);
	}
	words = build_words_from_buffer(frag, &wb, out_count);
	free(wb.buf);
	free(wb.splittable);
	if (words && wb.count > 0)
		*out_count = wb.count;
	return (words);
}

void	update_final_text(t_token *tok, t_proc_ctx *ctx)
{
	expand_fragments(tok, ctx->envp, ctx->state);
	free(tok->final_text);
	tok->final_text = concat_final_text(tok);
	/* If expansion produced an operator (like "|", ">", "<", ">>", "<<")
	   and the fragments are unquoted, update the token type so the parser
	   will treat it as an operator. This handles cases where a variable
	   expands to a metacharacter. */
	if (tok->final_text && tok->fragments
		&& detect_combined_quote(tok->fragments) == QUOTE_NONE)
	{
		t_token_type new_type;

		new_type = determine_token_type(tok->final_text, QUOTE_NONE);
		if (new_type != TOKEN_WORD)
			tok->type = new_type;
	}
}
