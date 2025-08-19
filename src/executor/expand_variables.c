/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/18 17:13:21 by djuarez          ###   ########.fr       */
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
		if (input[i] == '$')
		{
			piece = handle_dollar(input, &i, envp, state);
			if (!piece)
			{
				free(tmp);
				return (NULL);
			}
			tmp = str_append(tmp, piece);
			free (piece);
			if (!tmp)
				return (NULL);
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
