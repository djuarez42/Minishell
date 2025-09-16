/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 02:39:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 20:37:57 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*char **build_words_from_token(t_token *tok, int *out_count)
{
	t_fragment *frag;
	size_t total_len = 0;
	size_t pos = 0;
	char *S;
	unsigned char *splittable; 
	char **words = NULL;
	int count = 0;
	size_t i;

	if (!tok || !out_count)
		return NULL;
	frag = tok->fragments;
	while (frag)
	{
		if (frag->expanded_text)
			total_len += ft_strlen(frag->expanded_text);
		frag = frag->next;
	}
	S = malloc(total_len + 1);
	splittable = malloc(total_len);
	if (!S || !splittable)
	{
		free(S);
		free(splittable);
		*out_count = 0;
		return NULL;
	}
	frag = tok->fragments;
	pos = 0;
	while (frag)
	{
		if (frag->expanded_text)
		{
			size_t flen = ft_strlen(frag->expanded_text);
			int frag_splittable = 0;
			if (frag->quote_type == QUOTE_NONE && frag->text && frag->text[0] == '$')
				frag_splittable = 1;

			for (i = 0; i < flen; i++)
			{
				S[pos] = frag->expanded_text[i];
				splittable[pos] = frag_splittable ? 1 : 0;
				pos++;
			}
		}
		frag = frag->next;
	}
	S[pos] = '\0';
	i = 0;
	while (i < pos)
	{
		while (i < pos && ft_isspace((unsigned char)S[i]) && splittable[i])
			i++;
		if (i >= pos)
			break;
		count++;
		while (i < pos && !(ft_isspace((unsigned char)S[i]) && splittable[i]))
			i++;
	}
	if (count == 0)
	{
		int frag_count = 0;
		frag = tok->fragments;
		while (frag)
		{
			if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
				frag_count++;
			frag = frag->next;
		}
		if (frag_count > 0)
		{
			words = malloc(sizeof(char *) * (frag_count + 1));
			if (!words)
			{
				free(S);
				free(splittable);
				*out_count = 0;
				return NULL;
			}
			frag = tok->fragments;
			int wi = 0;
			while (frag)
			{
				if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
				{
					words[wi] = ft_strdup("");
					wi++;
				}
				frag = frag->next;
			}
			words[frag_count] = NULL;
			*out_count = frag_count;
			free(S);
			free(splittable);
			return words;
		}
		free(S);
		free(splittable);
		*out_count = 0;
		return NULL;
	}
	words = malloc(sizeof(char *) * (count + 1));
	if (!words)
	{
		free(S);
		free(splittable);
		*out_count = 0;
		return NULL;
	}
	i = 0;
	int wi = 0;
	while (i < pos && wi < count)
	{
		while (i < pos && ft_isspace((unsigned char)S[i]) && splittable[i])
			i++;
		if (i >= pos)
			break;
		size_t start = i;
		while (i < pos && !(ft_isspace((unsigned char)S[i]) && splittable[i]))
			i++;
		size_t wlen = i - start;
		words[wi] = ft_strndup(&S[start], wlen);
		wi++;
	}
	words[wi] = NULL;
	free(S);
	free(splittable);
	*out_count = count;
	return words;
}*/

static void	calc_total_len(t_fragment *frag, t_word_builder *wb)
{
	wb->total_len = 0;
	while (frag)
	{
		if (frag->expanded_text)
			wb->total_len += ft_strlen(frag->expanded_text);
		frag = frag->next;
	}
}

static void	copy_fragment_to_buffer(t_fragment *frag, t_word_builder *wb)
{
	size_t	i;
	size_t	flen;
	int		frag_splittable;

	flen = ft_strlen(frag->expanded_text);
	frag_splittable = 0;
	if (frag->quote_type == QUOTE_NONE
		&& frag->text
		&& frag->text[0] == '$')
		frag_splittable = 1;
	i = 0;
	while (i < flen)
	{
		wb->buf[wb->len] = frag->expanded_text[i];
		if (frag_splittable)
			wb->splittable[wb->len] = 1;
		else
			wb->splittable[wb->len] = 0;
		wb->len++;
		i++;
	}
}

void	fill_buffer_and_splittable(t_fragment *frag, t_word_builder *wb)
{
	wb->len = 0;
	while (frag)
	{
		if (frag->expanded_text)
			copy_fragment_to_buffer(frag, wb);
		frag = frag->next;
	}
	wb->buf[wb->len] = '\0';
}

/*static void	fill_buffer_and_splittable(t_fragment *frag, t_word_builder *wb)
{
	size_t	i;
	size_t	flen;
	int		frag_splittable;

	wb->len = 0;
	while (frag)
	{
		if (frag->expanded_text)
		{
			flen = ft_strlen(frag->expanded_text);
			frag_splittable = 0;
			if (frag->quote_type == QUOTE_NONE
				&& frag->text
				&& frag->text[0] == '$')
				frag_splittable = 1;
			i = 0;
			while (i < flen)
			{
				wb->buf[wb->len] = frag->expanded_text[i];
				if (frag_splittable)
					wb->splittable[wb->len] = 1;
				else
					wb->splittable[wb->len] = 0;
				wb->len++;
				i++;
			}
		}
		frag = frag->next;
	}
	wb->buf[wb->len] = '\0';
}*/

static void	count_words(t_word_builder *wb)
{
	size_t	i;

	i = 0;
	wb->count = 0;
	while (i < wb->len)
	{
		while (i < wb->len
			&& ft_isspace((unsigned char)wb->buf[i])
			&& wb->splittable[i])
			i++;
		if (i >= wb->len)
			break ;
		wb->count++;
		while (i < wb->len
			&& !(ft_isspace((unsigned char)wb->buf[i])
				&& wb->splittable[i]))
			i++;
	}
}

static int	count_quoted_fragments(t_fragment *frag)
{
	t_fragment	*cur;
	int			count;

	cur = frag;
	count = 0;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE
			|| cur->quote_type == QUOTE_DOUBLE)
			count++;
		cur = cur->next;
	}
	return (count);
}

static void	fill_quoted_words(t_fragment *frag, char **words)
{
	t_fragment	*cur;
	int			i;

	cur = frag;
	i = 0;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE
			|| cur->quote_type == QUOTE_DOUBLE)
		{
			words[i] = ft_strdup("");
			i++;
		}
		cur = cur->next;
	}
}

static char	**build_from_quoted_fragments(t_fragment *frag, int *out_count)
{
	char	**words;
	int		frag_count;

	frag_count = count_quoted_fragments(frag);
	if (frag_count == 0)
	{
		*out_count = 0;
		return (NULL);
	}
	words = malloc(sizeof(char *) * (frag_count + 1));
	if (!words)
	{
		*out_count = 0;
		return (NULL);
	}
	fill_quoted_words(frag, words);
	words[frag_count] = NULL;
	*out_count = frag_count;
	return (words);
}

/*static char	**build_from_quoted_fragments(t_fragment *frag, int *out_count)
{
	t_fragment *cur = frag;
	int frag_count = 0;
	int wi = 0;
	char **words;

	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE || cur->quote_type == QUOTE_DOUBLE)
			frag_count++;
		cur = cur->next;
	}
	if (frag_count == 0)
	{
		*out_count = 0;
		return NULL;
	}
	words = malloc(sizeof(char *) * (frag_count + 1));
	if (!words)
	{
		*out_count = 0;
		return NULL;
	}
	cur = frag;
	while (cur)
	{
		if (cur->quote_type == QUOTE_SINGLE || cur->quote_type == QUOTE_DOUBLE)
			words[wi++] = ft_strdup("");
		cur = cur->next;
	}
	words[frag_count] = NULL;
	*out_count = frag_count;
	return words;
}*/

static size_t	get_next_word_range(t_word_builder *wb, size_t start,
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

static char	**split_buffer_to_words(t_word_builder *wb)
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

/*static char	**split_buffer_to_words(t_word_builder *wb)
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
		while (i < wb->len && ft_isspace((unsigned char)wb->buf[i])
			&& wb->splittable[i])
			i++;
		if (i >= wb->len)
			break ;
		start = i;
		while (i < wb->len && !(ft_isspace((unsigned char)wb->buf[i])
				&& wb->splittable[i]))
			i++;
		wlen = i - start;
		words[wi] = ft_strndup(&wb->buf[start], wlen);
		wi++;
	}
	words[wi] = NULL;
	return (words);
}*/

/*char	**build_words_from_token(t_token *tok, int *out_count)
{
	t_fragment *frag;
	t_word_builder wb;
	char **words;

	if (!tok || !out_count)
		return NULL;
	frag = tok->fragments;
	calc_total_len(frag, &wb);
	wb.buf = malloc(wb.total_len + 1);
	wb.splittable = malloc(wb.total_len);
	if (!wb.buf || !wb.splittable)
	{
		free(wb.buf);
		free(wb.splittable);
		*out_count = 0;
		return NULL;
	}
	fill_buffer_and_splittable(frag, &wb);
	count_words(&wb);
	if (wb.count == 0)
	{
		words = build_from_quoted_fragments(frag, out_count);
		free(wb.buf);
		free(wb.splittable);
		return words;
	}
	words = split_buffer_to_words(&wb);
	free(wb.buf);
	free(wb.splittable);
	*out_count = wb.count;
	return words;
}*/

static char	**build_words_from_buffer(t_fragment *frag,
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
}
