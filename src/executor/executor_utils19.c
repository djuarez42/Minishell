/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils19.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/22 19:30:08 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_special_dollar_cases(const char *input, int *i,
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
	else if (input[start] == '\0')
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	return (NULL);
}

static char	*handle_invalid_var_start(int *i)
{
	*i = *i + 1;
	return (ft_strdup("$"));
}

static char	*handle_normal_variable(const char *input, int *i, char **envp)
{
	int		start;
	int		len;
	char	*name;
	char	*res;

	start = *i + 1;
	len = skip_variable_name(input + start);
	if (len == 0)
		return (handle_invalid_var_start(i));
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
	char	*res;

	if (!input || !i)
		return (NULL);
	res = handle_special_dollar_cases(input, i, state);
	if (res)
		return (res);
	if (!is_var_start(input[*i + 1]))
		return (handle_invalid_var_start(i));
	return (handle_normal_variable(input, i, envp));
}
