/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils7.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:14:41 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 21:27:16 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token_type	check_simple_tokens(char *str);

char	*concat_fragments(t_fragment *frag)
{
	size_t	len;
	char	*res;

	if (!frag)
		return (NULL);
	len = calc_total_length(frag);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	copy_fragments_to_buffer(frag, res);
	return (res);
}

// helper functions moved to lexer_utils6.c to satisfy Norminette function count

t_token_type	determine_token_type(char *str, t_quote_type quote)
{
	if (!str)
		return (TOKEN_WORD);
	if (quote != QUOTE_NONE)
		return (TOKEN_WORD);
	return (check_simple_tokens(str));
}

static t_token_type	check_simple_tokens(char *str)
{
	if (str[0] == '<' && str[1] == '<' && str[2] == '\0')
		return (TOKEN_HEREDOC);
	if (str[0] == '>' && str[1] == '>' && str[2] == '\0')
		return (TOKEN_APPEND);
	if (str[0] == '<' && str[1] == '\0')
		return (TOKEN_REDIRECT_IN);
	if (str[0] == '>' && str[1] == '\0')
		return (TOKEN_REDIRECT_OUT);
	if (str[0] == '>' && str[1] == '|' && str[2] == '\0')
		return (TOKEN_REDIRECT_OUT);
	if (str[0] == '|' && str[1] == '\0')
		return (TOKEN_PIPE);
	return (TOKEN_WORD);
}
