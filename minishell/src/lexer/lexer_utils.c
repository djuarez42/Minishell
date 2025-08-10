/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 11:03:33 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/06 20:24:30 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

int	is_blank(const char *s)
{
	while (*s)
	{
		if (*s != ' ')
			return (0);
		s++;
	}
	return (1);
}

void	free_tokens(char **tokens, int max)
{
	int	i;

	if (!tokens)
		return ;
	i = 0;
	if (max < 0)
	{
		while (tokens[i])
		{
			free(tokens[i]);
			tokens[i] = NULL;
			i++;
		}
	}
	else
	{
		while (i < max && tokens[i])
		{
			free(tokens[i]);
			tokens[i] = NULL;
			i++;
		}
	}
	free(tokens);
}
