/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_mixed_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 21:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/27 21:00:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"

/*
 * This function handles expansion for strings with mixed quote types
 * like ""'$USER'"" where single-quoted parts should not expand variables
 */
char	*expand_mixed_quotes(const char *input, char **envp, t_exec_state *state)
{
	int		i;
	char	*result;
	char	*segment;
	char	quote_char;
	int		start;
	int		len;

	if (!input)
		return (NULL);
	i = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	
	while (input[i])
	{
		if (input[i] == '\'' || input[i] == '\"')
		{
			quote_char = input[i];
			start = i + 1; // Skip opening quote
			i++; // Move past opening quote
			
			// Find closing quote
			while (input[i] && input[i] != quote_char)
				i++;
			
			if (input[i] == quote_char)
			{
				// Extract content between quotes
				len = i - start;
				if (len > 0)
				{
					segment = ft_substr(input, start, len);
					if (!segment)
					{
						free(result);
						return (NULL);
					}
					
					// Only expand if not single-quoted
					if (quote_char == '\'')
					{
						// Single quotes: no expansion - keep content as is
						char *temp = str_append(result, segment);
						free(result);
						result = temp;
					}
					else
					{
						// Double quotes: expand variables
						char *expanded = expand_variables(segment, envp, state, QUOTE_DOUBLE);
						if (!expanded)
						{
							free(segment);
							free(result);
							return (NULL);
						}
						char *temp = str_append(result, expanded);
						free(result);
						free(expanded);
						result = temp;
					}
					free(segment);
				}
				i++; // Skip closing quote
			}
			else
			{
				// Unclosed quote - treat as plain text
				char temp_char[2] = {input[i - 1], '\0'};
				char *temp = str_append(result, temp_char);
				free(result);
				result = temp;
			}
		}
		else
		{
			// Plain text - collect until next quote or end
			start = i;
			while (input[i] && input[i] != '\'' && input[i] != '\"')
				i++;
			
			if (i > start)
			{
				segment = ft_substr(input, start, i - start);
				if (!segment)
				{
					free(result);
					return (NULL);
				}
				
				char *expanded = expand_variables(segment, envp, state, QUOTE_NONE);
				if (!expanded)
				{
					free(segment);
					free(result);
					return (NULL);
				}
				char *temp = str_append(result, expanded);
				free(result);
				free(expanded);
				result = temp;
				free(segment);
			}
		}
		
		if (!result)
			return (NULL);
	}
	
	return (result);
}
