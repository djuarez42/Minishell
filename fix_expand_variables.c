/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 14:30:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to specifically handle $"string" pattern
static char *handle_dollar_string(const char *input, int *i)
{
    char *result;
    int start, end;
    
    // Skip the $ character
    (*i)++;
    
    // Skip the opening quote
    (*i)++;
    
    // Find the end of the quoted string
    start = *i;
    end = start;
    while (input[end] && input[end] != '"')
        end++;
    
    // Extract the content of the quotes without expanding variables
    result = ft_substr(input, start, end - start);
    if (!result)
        return (NULL);
    
    // Skip past the closing quote if present
    if (input[end] == '"')
        *i = end + 1;
    else
        *i = end;
    
    return (result);
}

// Helper function to check for $"string" pattern
static int is_dollar_string(const char *input, int pos)
{
    if (!input || !input[pos] || !input[pos + 1])
        return (0);
    
    return (input[pos] == '$' && input[pos + 1] == '"');
}

char	*expand_variables(const char *input, char **envp, t_exec_state *state,
	t_quote_type quote)
{
	int		i;
	char	*tmp;
	char	*piece;

	if (!input)
		return (NULL);
	i = 0;
	tmp = NULL;
	piece = NULL;
	while (input[i])
	{
		// Check for backslash escaping a $
		if (input[i] == '\\' && input[i+1] == '$')
		{
			// Add just the literal $ to output and skip both characters
			piece = ft_strdup("$");
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free(piece);
			i += 2; // Skip both backslash and dollar
		}
		else if (input[i] == '$')
		{
			// Check for $"string" syntax first
			if (is_dollar_string(input, i))
			{
				// Special handling for $"string" syntax - keep the literal string
				piece = handle_dollar_string(input, &i);
			}
			else if (quote == QUOTE_DOUBLE)
			{
				// In double quotes, variables are expanded but spaces are preserved
				piece = handle_dollar_quotes_fix(input, &i, envp, state);
			}
			else if (quote == QUOTE_SINGLE)
			{
				// In single quotes, variables are not expanded
				piece = ft_strdup("$");
				i++;
			}
			else
			{
				// Outside quotes, variables are expanded and splitting may occur on spaces
				piece = handle_dollar_quotes_fix(input, &i, envp, state);
			}
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free(piece);
		}
		else if (quote == QUOTE_SINGLE && input[i] == '\\')
		{
			// In single quotes, backslashes are treated literally
			piece = ft_strdup("\\");
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free(piece);
			i++;
		}
		else
		{
			// Any other character
			piece = ft_strdupc(input[i]);
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free(piece);
			i++;
		}
	}
	return (tmp);
}
