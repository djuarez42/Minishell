/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preprocess_dollar_quotes.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 20:45:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/28 20:59:46 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/*
** Handle $"content" and $'content' cases where $ should disappear
** This processes the token value BEFORE normal expansion
*/
char	*preprocess_dollar_quotes(const char *input)
{
	char	*result;
	int		i;
	int		j;
	int		len;

	if (!input)
		return (NULL);
	
	len = ft_strlen(input);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	
	i = 0;
	j = 0;
	while (input[i])
	{
		if (input[i] == '$' && (input[i + 1] == '"' || input[i + 1] == '\''))
		{
			// Skip the $, keep everything else
			i++;
		}
		else
		{
			result[j] = input[i];
			j++;
			i++;
		}
	}
	result[j] = '\0';
	return (result);
}
