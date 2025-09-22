/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils18.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 05:05:24 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 05:08:19 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_name_char(int c, int start)
{
	if (start)
		return (ft_isalpha(c) || c == '_');
	return (ft_isalnum(c) || c == '_');
}

int	env_identifier_valid(const char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!is_name_char(name[0], 1))
		return (0);
	i = 1;
	while (name[i])
	{
		if (!is_name_char(name[i], 0))
			return (0);
		i++;
	}
	return (1);
}

int	env_find_index(char **envp, const char *name)
{
	int		len;
	int		i;

	if (!envp || !name)
		return (-1);
	len = ft_strlen(name);
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], name, len) && envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

char	*env_get_value(char **envp, const char *name)
{
	int	idx;

	idx = env_find_index(envp, name);
	if (idx < 0)
		return (NULL);
	return (ft_strchr(envp[idx], '=') + 1);
}
