/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/27 20:36:30 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_variables(const char *input, char **envp, t_exec_state *state,
							t_quote_type quote)
{
	int i;
	char *tmp;
	char *piece;

	if (!input)
		return (ft_strdup(""));

	tmp = NULL;
	i = 0;

	while (input[i])
	{
		if (input[i] == '$' && quote != QUOTE_SINGLE)
		{
			piece = handle_dollar(input, &i, envp, state);
			if (!piece)
				piece = ft_strdup("");
			tmp = str_append(tmp, piece);
			free(piece);
		}
		else
		{
			tmp = extract_plain_text(input, &i, tmp);
		}
	}

	if (!tmp)
		tmp = ft_strdup("");

	return (tmp);
}

int expand_cmd_inplace(t_cmd *cur, char **envp, t_exec_state *state)
{
	size_t			i;
	char			*expanded;
	t_quote_type	quote_type;

	i = 0;
	while (cur->argv && cur->argv[i])
	{
		if (cur->argv_quote && cur->argv_quote[i] == QUOTE_SINGLE)
		{
			i++;
			continue ;
		}

		quote_type = QUOTE_NONE;
		if (cur->argv_quote)
			quote_type = cur->argv_quote[i];

		expanded = expand_variables(cur->argv[i], envp, state, quote_type);
		if (!expanded)
			return (-1);

		free(cur->argv[i]);
		cur->argv[i] = expanded;
		i++;
	}

	if (expand_redirs(cur->redirs, envp, state) == -1)
		return (-1);

	return (0);
}
