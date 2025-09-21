/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_quotes_fix.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 21:38:49 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*dq_handle_edge_cases(const char *input, int *i,
			t_exec_state *state)
{
	int	start;

	start = *i + 1;
	if (input[start] == '"')
	{
		*i = start;
		return (ft_strdup(""));
	}
	if (input[start] == '?')
	{
		*i = start + 1;
		return (expand_exit_status(state));
	}
	if (input[start] == '$')
	{
		*i = start + 1;
		return (ft_strdup("$"));
	}
	if (input[start] == '\0')
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	return (NULL);
}

static char	*dq_expand_variable(const char *input, int start, int *i,
			char **envp)
{
	int		len;
	char	*name;
	char	*res;

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

char	*handle_dollar_quotes_fix(const char *input, int *i, char **envp,
			t_exec_state *state)
{
	int		start;
	char	*res;

	if (!input || !i)
		return (NULL);
	res = dq_handle_edge_cases(input, i, state);
	if (res)
		return (res);
	start = *i + 1;
	return (dq_expand_variable(input, start, i, envp));
}
