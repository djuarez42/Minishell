/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 14:09:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/01 17:17:17 by djuarez          ###   ########.fr       */
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

/*
void	print_fragments(t_fragment *fragments)
{
	int			i;
	const char	*space_str;

	i = 0;
	while (fragments)
	{
		ft_putstr_fd("Fragment ", 1);
		ft_putstr_fd(ft_itoa(i), 1);
		ft_putendl_fd(":", 1);
		ft_putstr_fd("  text           : \"", 1);
		ft_putstr_fd(fragments->text, 1);
		ft_putendl_fd("\"", 1);
		ft_putstr_fd("  quote_type     : ", 1);
		ft_putstr_fd(ft_itoa(fragments->quote_type), 1);
		ft_putendl_fd("", 1);
		if (fragments->has_space_after)
			space_str = "true";
		else
			space_str = "false";
		ft_putstr_fd("  has_space_after: ", 1);
		ft_putstr_fd((char *)space_str, 1);
		ft_putendl_fd("", 1);
		ft_putstr_fd("  next           : ", 1);
		ft_putstr_fd(ft_itoa((long long)(fragments->next)), 1);
		ft_putendl_fd("", 1);
		fragments = fragments->next;
		i++;
	}
}

void	print_tokens_raw(t_token *tokens)
{
	int			i;
	const char	*final_text;

	i = 0;
	while (tokens)
	{
		ft_putstr_fd("Token ", 1);
		ft_putstr_fd(ft_itoa(i), 1);
		ft_putendl_fd(":", 1);
		ft_putstr_fd("  type           : ", 1);
		ft_putstr_fd(ft_itoa(tokens->type), 1);
		ft_putendl_fd("", 1);
		   if (tokens->has_space_before) {
				ft_putstr_fd("  has_space_before: true", 1);
			   ft_putendl_fd("", 1);
		   } else {
			   ft_putstr_fd("  has_space_before: false", 1);
			   ft_putendl_fd("", 1);
		   }
		if (tokens->final_text)
			final_text = tokens->final_text;
		else
			final_text = "(null)";
		ft_putstr_fd("  final_text     : \"", 1);
		ft_putstr_fd((char *)final_text, 1);
		ft_putendl_fd("\"", 1);
		print_fragments(tokens->fragments);
		ft_putendl_fd("", 1);
		tokens = tokens->next;
		i++;
	}
}
*/

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
	ft_memcpy(frag->text, start, len);
	frag->text[len] = '\0';
	frag->expanded_text = NULL;
	frag->quote_type = qtype;
	frag->has_space_after = space_after;
	frag->is_variable = false;
	frag->next = NULL;
	return (frag);
}
