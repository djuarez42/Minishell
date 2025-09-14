/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 20:40:15 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 00:06:14 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"



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
