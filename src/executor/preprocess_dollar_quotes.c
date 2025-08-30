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
** Special handling for $"content" cases.
** Unlike in normal expansion, we need to preserve the content literally.
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
		// Detect $"string" pattern
		if (input[i] == '$' && input[i + 1] == '"')
		{
			// Mark this with a special flag
			// Skip the $ but keep the quotes
			i++;
			
			// Copy the open quote
			result[j++] = input[i++];
			
			// Copy everything until closing quote
			while (input[i] && input[i] != '"')
			{
				result[j++] = input[i++];
			}
			
			// Copy the closing quote if found
			if (input[i] == '"')
				result[j++] = input[i++];
		}
		else
		{
			result[j++] = input[i++];
		}
	}
	result[j] = '\0';
	return (result);
}
