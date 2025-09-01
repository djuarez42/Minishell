/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preprocess_dollar_quotes.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 20:45:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/01 12:30:45 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

/*
** Handles $"string" syntax according to bash behavior.
** In bash, $"string" is locale translation, but we implement it simply:
** - Keep the literal string (without variable expansion)
** - This matches bash behavior when no translations are available
*/
char	*preprocess_dollar_quotes(const char *input)
{
	char	*result;
	int		i;
	int		j;
	int		len;

	if (!input)
		return (NULL);
	
	len = ft_strlen(input) * 2; // Allocate extra space for escapes
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	
	i = 0;
	j = 0;
	
	while (input[i])
	{
		// Special case: $"string" - treat contents literally as in bash
		if (input[i] == '$' && input[i + 1] == '"')
		{
			// Replace the $ with a unique marker pattern
			result[j++] = '"'; // Start with a quote
			i += 2; // Skip both $ and "
			
			// Copy content literally
			while (input[i] && input[i] != '"')
			{
				// Escape any variables to prevent expansion
				if (input[i] == '$')
				{
					result[j++] = '\\'; // Add escape character
				}
				result[j++] = input[i++];
			}
			
			// Copy closing quote
			if (input[i] == '"')
			{
				result[j++] = input[i++];
			}
		}
		else
		{
			// Regular character
			result[j++] = input[i++];
		}
	}
	
	result[j] = '\0';
	return (result);
}
