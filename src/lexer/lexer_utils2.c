/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 16:45:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 20:25:17 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_fragment	*new_fragment(const char *start, size_t len, t_quote_type qtype,
				bool space_after)
{
	t_fragment	*frag;

	frag = malloc(sizeof(t_fragment));
	if (!frag)
		return (NULL);
	frag->text = malloc(len + 1);
	if (!frag->text)
	{
		free(frag);
		return (NULL);
	}
	memcpy(frag->text, start, len);
	frag->text[len] = '\0';
	frag->expanded_text = NULL;
	frag->quote_type = qtype;
	frag->has_space_after = space_after;
	frag->is_variable = false;
	frag->next = NULL;
	return (frag);
}

void	append_fragment(t_fragment **head, t_fragment *frag)
{
	t_fragment	*tmp;

	tmp = *head;
	if (!frag)
		return ;
	if (!*head)
	{
		*head = frag;
		return ;
	}
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = frag;
}

void	print_fragments(t_fragment *fragments)
{
	int			i;
	const char	*space_str;

	i = 0;
	while (fragments)
	{
		printf("Fragment %d:\n", i);
		printf("  text           : \"%s\"\n", fragments->text);
		printf("  quote_type     : %d\n", fragments->quote_type);
		if (fragments->has_space_after)
			space_str = "true";
		else
			space_str = "false";
		printf("  has_space_after: %s\n", space_str);
		printf("  next           : %p\n\n", (void *)fragments->next);
		fragments = fragments->next;
		i++;
	}
}

void	print_tokens_raw(t_token *tokens)
{
	int			i;
	int			j;
	t_fragment	*frag;
	const char	*space_str;
	const char	*final_text;

	i = 0;
	while (tokens)
	{
		printf("Token %d:\n", i);
		printf("  type           : %d\n", tokens->type);
		if (tokens->has_space_before)
			printf("  has_space_before: true\n");
		else
			printf("  has_space_before: false\n");
		if (tokens->final_text)
			final_text = tokens->final_text;
		else
			final_text = "(null)";
		printf("  final_text     : \"%s\"\n", final_text);
		frag = tokens->fragments;
		j = 0;
		while (frag)
		{
			if (frag->has_space_after)
				space_str = "true";
			else
				space_str = "false";
			printf("    Fragment %d: text=\"%s\", quote_type=%d, "
				"has_space_after=%s\n",
				j,
				frag->text,
				frag->quote_type,
				space_str);
			frag = frag->next;
			j++;
		}
		printf("\n");
		tokens = tokens->next;
		i++;
	}
}

t_fragment	*handle_backslashes(const char *text, int *i)
{
	int			start;
	int			count;
	char		next;
	int			keep;
	int			buflen;
	char		*buf;
	int			j;
	bool		space_after;
	t_fragment	*f;

	start = *i;
	count = 0;
	while (text[*i] && text[*i] == '\\')
	{
		count++;
		(*i)++;
	}
	if (count == 0)
		return (NULL);
	next = text[*i];
	if (next == '$')
	{
		keep = count / 2;
		if (count % 2 == 0)
		{
			if (keep > 0)
			{
				buf = malloc((size_t)keep + 1);
				if (!buf)
					return (NULL);
				j = 0;
				while (j < keep)
				{
					buf[j] = '\\';
					j++;
				}
				buf[keep] = '\0';
				space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
				f = new_fragment(buf, keep, QUOTE_NONE, space_after);
				free(buf);
				return (f);
			}
			return (NULL);
		}
		else
		{
			buflen = keep + 1;
			buf = malloc((size_t)buflen + 1);
			if (!buf)
				return (NULL);
			j = 0;
			while (j < keep)
			{
				buf[j] = '\\';
				j++;
			}
			buf[keep] = '$';
			buf[buflen] = '\0';
			(*i)++;
			space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
			f = new_fragment(buf, buflen, QUOTE_NONE, space_after);
			free(buf);
			return (f);
		}
	}
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	f = new_fragment(&text[start], (size_t)count, QUOTE_NONE, space_after);
	return (f);
}

t_fragment *handle_spaces(const char *text, int *i)
{
	while (text[*i] && ft_isspace((unsigned char)text[*i]))
		(*i)++;
	return (NULL);
}

t_fragment *handle_backslashes_wrapper(const char *text, int *i)
{
	return handle_backslashes(text, i);
}

t_fragment	*handle_dollar_string(const char *text, int *i)
{
	char		quote;
	int			start;
	int			len;
	bool		space_after;
	t_fragment	*frag;

	quote = text[*i + 1];
	*i += 2;
	start = *i;
	while (text[*i] && text[*i] != quote)
	{
		if (quote == '"' && text[*i] == '\\' && text[*i + 1])
			*i += 2;
		else
			(*i)++;
	}
	len = *i - start;
	space_after = text[*i + 1] && ft_isspace((unsigned char)text[*i + 1]);
	if (quote == '\'')
		frag = new_fragment(&text[start], (size_t)len,
				QUOTE_DOLLAR, space_after);
	else
		frag = new_fragment(&text[start], (size_t)len,
				QUOTE_DOUBLE, space_after);
	if (text[*i] == quote)
		(*i)++;
	return (frag);
}


t_fragment	*handle_single_quotes(const char *text, int *i)
{
	int			start;
	int			len;
	bool		space_after;
	t_fragment	*frag;

	(*i)++;
	start = *i;
	while (text[*i] && text[*i] != '\'')
		(*i)++;
	len = *i - start;
	space_after = text[*i + 1] && ft_isspace((unsigned char)text[*i + 1]);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_SINGLE, space_after);
	if (text[*i] == '\'')
		(*i)++;
	return (frag);
}

t_fragment	*handle_double_quotes(const char *text, int *i)
{
	int			start;
	int			len;
	bool		space_after;
	t_fragment	*frag;

	(*i)++;
	start = *i;
	while (text[*i] && text[*i] != '"')
	{
		if (text[*i] == '\\' && text[*i + 1])
			*i += 2;
		else
			(*i)++;
	}
	len = *i - start;
	space_after = text[*i + 1] && ft_isspace((unsigned char)text[*i + 1]);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_DOUBLE, space_after);
	if (text[*i] == '"')
		(*i)++;
	return (frag);
}

t_fragment	*handle_operators(const char *text, int *i)
{
	int			start;
	int			len;
	bool		space_after;

	start = *i;
	if (text[*i] == '<' && text[*i + 1] == '<')
	{
		*i += 2;
	}
	else if (text[*i] == '>' && text[*i + 1] == '>')
	{
		*i += 2;
	}
	else
	{
		(*i)++;
	}
	len = *i - start;
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	return (new_fragment(&text[start], (size_t)len, QUOTE_NONE, space_after));
}
t_fragment *handle_generic_text(const char *text, int *i)
{
	t_fragment	*frag;
	int			start;
	int			len;
	bool		space_after;

	frag = NULL;
	start = *i;
	while (text[*i] && !ft_isspace((unsigned char)text[*i]) && text[*i] != '\''
		&& text[*i] != '"' && text[*i] != '|'
		&& text[*i] != '<' && text[*i] != '>')
	{
		(*i)++;
	}
	len = *i - start;
	space_after = text[*i] && ft_isspace((unsigned char)text[*i]);
	frag = new_fragment(&text[start], (size_t)len, QUOTE_NONE, space_after);
	return (frag);
}

t_fragment *parse_mixed_fragments(const char *text)
{
	t_fragment	*fragments;
	t_fragment	*frag;
	int			i;

	i = 0;
	fragments = NULL;
	while (text[i])
	{
		handle_spaces(text, &i);
		if (!text[i])
			break ;
		frag = NULL;
		if (text[i] == '\\')
			frag = handle_backslashes_wrapper(text, &i);
		else if (is_dollar_string(text, i))
			frag = handle_dollar_string(text, &i);
		else if (text[i] == '\'')
			frag = handle_single_quotes(text, &i);
		else if (text[i] == '"')
			frag = handle_double_quotes(text, &i);
		else if (text[i] == '|' || text[i] == '<' || text[i] == '>')
			frag = handle_operators(text, &i);
		else
			frag = handle_generic_text(text, &i);
		if (frag)
			append_fragment(&fragments, frag);
	}
	return (fragments);
}
