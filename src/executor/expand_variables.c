/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 01:15:28 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
			// Pass the quote type to the dollar handling function
			// This will ensure proper space preservation in expansions
			if (quote == QUOTE_DOUBLE)
			{
				// In double quotes, variables are expanded but spaces are preserved
				piece = handle_dollar(input, &i, envp, state);
			}
			else
			{
				// Outside quotes, standard expansion with word splitting
				piece = handle_dollar(input, &i, envp, state);
			}
			
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
		else
		{
			tmp = extract_plain_text(input, &i, tmp);
			if (!tmp)
				return (NULL);
		}
	}
	return (tmp);
}

/*
 * Specially handle mixed quotes within a command argument
 * This function detects patterns like "1"$X"2" and ensures
 * they are processed correctly as a single concatenated string
 */
char *process_mixed_quotes_argv(char *arg, t_quote_type quote_type, char **envp, t_exec_state *state)
{
	// If not a mixed quotes case, just use regular expansion
	if (quote_type != QUOTE_MIXED)
		return expand_variables(arg, envp, state, quote_type);
	
	// For mixed quotes, use special handling that preserves spaces in variables
	return expand_mixed_quotes(arg, envp, state);
}

int	expand_cmd_inplace(t_cmd *cur, char **envp, t_exec_state *state)
{
	if (expand_argv(cur->argv, cur->argv_quote, envp, state) == -1)
		return (-1);
	if (expand_redirs(cur->redirs, envp, state) == -1)
		return (-1);
	return (0);
}
