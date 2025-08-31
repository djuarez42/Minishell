#!/bin/bash

# Fix implementation for variable expansion in quotes
echo "Implementing variable expansion fix with fixed warnings..."

# Modify expand_variables.c
cat > src/executor/expand_variables.c << 'EOF'
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 19:30:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Fixed expand_variables function with proper handling of $? in quotes
 */
char	*expand_variables(const char *input, char **envp, t_exec_state *state,
	t_quote_type quote)
{
	int		i;
	char	*tmp;
	char	*piece;

	(void)quote; // Suppress unused parameter warning
	if (!input)
		return (NULL);
	i = 0;
	tmp = NULL;
	piece = NULL;
	while (input[i])
	{
		if (input[i] == '\\' && input[i+1] == '$')
		{
			piece = ft_strdup("$");
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free(piece);
			i += 2; // Skip backslash and dollar
		}
		else if (input[i] == '$' && input[i+1] == '?')
		{
			// Direct handling of $? for reliable expansion
			piece = expand_exit_status(state);
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free(piece);
			i += 2; // Skip $ and ?
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
EOF

echo "Fix implemented. Rebuilding..."
make
