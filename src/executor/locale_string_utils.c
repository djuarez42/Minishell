/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locale_string_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 23:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/30 22:22:06 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Handle locale-specific translation of strings ($"string")
 * In minishell, we simply return the string unchanged as per bash behavior
 */
char	*handle_locale_string(const char *input, int *i)
{
	int		start;
	char	*str;
	
	// Skip the $ character
	(*i)++;
	
	// Skip the opening double quote
	if (input[*i] == '"')
		(*i)++;
	else
		return (ft_strdup("$")); // Not a locale string, just return $
	
	// Find closing quote
	start = *i;
	while (input[*i] && input[*i] != '"')
		(*i)++;
	
	// Extract the string between quotes
	str = ft_substr(input, start, *i - start);
	if (!str)
		return (NULL);
	
	// Skip the closing quote if present
	if (input[*i] == '"')
		(*i)++;
	
	// Return the string unchanged (no locale translation)
	return (str);
}
