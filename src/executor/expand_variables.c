/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/28 19:41:57 by ekakhmad         ###   ########.fr       */
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
		// Only process $ if not in single quotes
		if (quote != QUOTE_SINGLE)
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
			else
			{
				tmp = extract_plain_text(input, &i, tmp);
				if (!tmp)
					return (NULL);
			}
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

int	expand_cmd_inplace(t_cmd *cur, char **envp, t_exec_state *state)
{
	if (expand_argv(cur->argv, cur->argv_quote, envp, state) == -1)
		return (-1);
	// Note: redirection expansion moved to handle_redirections_and_quotes
	// to follow proper flowchart order
	return (0);
}
