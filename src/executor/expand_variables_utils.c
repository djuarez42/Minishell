/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 19:18:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/15 18:40:52 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_var_start(int c)
{
	if (ft_isalpha(c) || c == '_')
		return (1);
	return (0);
}

int	is_var_char(int c)
{
	if (ft_isalnum(c) || c == '_')
		return (1);
	return (0);
}

int	skip_variable_name(const char *s)
{
	int	i;

	if (!s || !*s || !is_var_start((unsigned char)s[0]))
		return (0);
	i = 1;
	while (s[i] && is_var_char((unsigned char)s[i]))
		i++;
	return (i);
}

char	*expand_exit_status(t_exec_state *state)
{
	int		code;
	char	*num;

	code = 0;
	if (state)
		code = state->last_status;
	num = ft_itoa(code);
	
	// Don't add a space after the exit status
	// Return the raw value
	return (num);
}

char	*expand_env_var(const char *name, char **envp)
{
	char	*val;

	if (!name || !*name)
		return (ft_strdup(""));
	val = env_get_value(envp, name);
	if (val)
		return (ft_strdup(val));
	return (ft_strdup(""));
}
