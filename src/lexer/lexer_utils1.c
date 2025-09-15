/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 14:09:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 21:50:15 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*remove_quotes(char *str)
{
	int		len;
	char	*result;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len > 1 && str[0] == '"' && str[len - 1] == '"')
		result = ft_substr(str, 1, len - 2);
	else
		result = ft_strdup(str);
	return (result);
}

t_token	*create_token(t_token_type type, bool space_before)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->type = type;
	tok->fragments = NULL;
	tok->has_space_before = space_before;
	tok->final_text = NULL;
	tok->next = NULL;
	return (tok);
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
