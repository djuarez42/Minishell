/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_quotes_fix.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 15:26:12 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *handle_dollar_quotes_fix(const char *input, int *i, char **envp, t_exec_state *state)
{
	int		start;
	int		len;
	char	*res;
	char	*name;

	if (!input || !i)
		return (NULL);
	start = *i + 1;
	if (input[start] == '"')
	{
		*i = start;
		return (ft_strdup(""));
	}
	else if (input[start] == '?')
	{
		*i = start + 1;
		return (expand_exit_status(state));
	}
	else if (input[start] == '$')
	{
		*i = start + 1;
		return (ft_strdup("$"));
	}
	if (input[start] == '\0')
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	if (!is_var_start(input[start]))
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	len = skip_variable_name(input + start);
	if (len == 0)
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	name = ft_substr(input, start, len);
	if (!name)
		return (NULL);
	res = expand_env_var(name, envp);
	free(name);
	*i = start + len;
	return (res);
}
