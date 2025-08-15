/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 20:40:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/15 18:45:16 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

int	is_quote(char c)
{
	return (c == '\'' || c == '\"');
}

int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	operator_len(const char *s)
{
	if ((s[0] == '>' && s[1] == '>') || (s[0] == '<' && s[1] == '<'))
		return (2);
	return (1);
}

int	skip_spaces(const char *s, int i)
{
	while (s[i])
	{
		if (!ft_isspace((unsigned char)s[i]))
			break ;
		i++;
	}
	return (i);
}
