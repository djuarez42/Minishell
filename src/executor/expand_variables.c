/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 16:26:54 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Forward declaration for function defined in expand_variables_utils2.c
char	*create_backslash_string(int count);
void	handle_backslash_dollar_parity(const char *input, int *i, 
										int *backslashes_out, bool *should_expand);

char	*expand_variables(const char *input, char **envp, t_exec_state *state,
								t_quote_type quote)
{
	int		i;
	char	*tmp;
	char	*piece;

	if (!input)
		return (NULL);
	
	// Special handling for $"content" and $'content' patterns
	// Only apply this for simple cases without additional $ characters
	if (quote != QUOTE_NONE && input[0] == '$' && input[1] != '\0')
	{
		// Check if there are more $ characters after the first one
		int has_more_dollars = 0;
		int j = 1;
		while (input[j])
		{
			if (input[j] == '$')
			{
				has_more_dollars = 1;
				break;
			}
			j++;
		}
		
		// Only skip the $ if this is a simple case like $"HOME" without additional variables
		if (!has_more_dollars)
		{
			// For $"HOME" → return "HOME", for $'HOME' → return "HOME"
			return (ft_strdup(input + 1)); // Skip the $ and return the rest
		}
		// For complex cases like $"HOME"$USER, fall through to normal processing
		// but we need to handle the first $ specially
	}
	
	i = 0;
	tmp = NULL;
	piece = NULL;
	while (input[i])
	{
		// Only process $ if not in single quotes
		if (quote != QUOTE_SINGLE)
		{
			// Check for backslash sequences
			if (input[i] == '\\')
			{
				int temp_i = i;
				int backslashes_out;
				bool should_expand;
				handle_backslash_dollar_parity(input, &temp_i, &backslashes_out, &should_expand);
				
				if (should_expand || backslashes_out > 0)
				{
					// Output the appropriate number of backslashes
					if (backslashes_out > 0)
					{
						piece = create_backslash_string(backslashes_out);
						if (!piece)
						{
							free(tmp);
							return (NULL);
						}
						tmp = str_append(tmp, piece);
						free(piece);
					}
					
					// Handle the dollar sign
					if (should_expand)
					{
						// Expand the variable
						piece = handle_dollar(input, &temp_i, envp, state);
						if (!piece)
						{
							free(tmp);
							return (NULL);
						}
						tmp = str_append(tmp, piece);
						free(piece);
					}
					else
					{
						// Add literal dollar sign
						piece = ft_strdup("$");
						if (!piece)
						{
							free(tmp);
							return (NULL);
						}
						tmp = str_append(tmp, piece);
						free(piece);
					}
					
					i = temp_i; // Update position
				}
				else
				{
					// Not a backslash-dollar sequence, handle as literal backslash
					piece = ft_strdup("\\");
					if (!piece)
					{
						free(tmp);
						return (NULL);
					}
					tmp = str_append(tmp, piece);
					free(piece);
					i = temp_i; // Use the updated position from handle_backslash_dollar_parity
				}
			}
			else if (input[i] == '$')
			{
				// Special case: if this is the first $ in a quoted context, skip it
				if (i == 0 && quote != QUOTE_NONE && input[i+1] != '\0' && 
					(input[i+1] != '$' && input[i+1] != '?' && !ft_isdigit((unsigned char)input[i+1])))
				{
					// Skip the first $ in $"content" patterns
					i++;
				}
				else
				{
					piece = handle_dollar(input, &i, envp, state);
					if (!piece)
					{
						free(tmp);
						return (NULL);
					}
					tmp = str_append(tmp, piece);
					if (!tmp)
					{
						free(piece);
						return (NULL);
					}
					free(piece);
				}
			}
			else
			{
				tmp = extract_plain_text(input, &i, tmp);
				if (!tmp)
					return (NULL);
			}
		}
		else // quote == QUOTE_SINGLE
		{
			// Special case for single quotes: handle $'content' pattern
			if (i == 0 && input[i] == '$' && input[i+1] != '\0')
			{
				// Skip the first $ in $'content' patterns
				i++;
			}
			else
			{
				tmp = extract_plain_text(input, &i, tmp);
				if (!tmp)
					return (NULL);
			}
		}
	}
	return (tmp);
}

int	expand_cmd_inplace(t_cmd *cur, char **envp, t_exec_state *state)
{
	if (expand_argv(cur->argv, cur->argv_quote, envp, state) == -1)
		return (-1);
	// Note: redirection expansion moved to handle_redirections_and_quotes
	// to follow proper flowchart order
	return (0);
}
