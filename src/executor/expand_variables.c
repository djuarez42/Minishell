/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/27 20:16:49 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_variables(const char *input, char **envp, t_exec_state *state)
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
		// Handle backslash escaping a $
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
	return (tmp);
}

int	expand_cmd_inplace(t_cmd *cur, char **envp, t_exec_state *state)
{
	if (expand_argv(cur->argv, cur->argv_quote, envp, state) == -1)
		return (-1);
	if (expand_redirs(cur->redirs, envp, state) == -1)
		return (-1);
	return (0);
}
