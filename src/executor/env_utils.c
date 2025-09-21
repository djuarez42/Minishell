/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 19:53:41 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 21:40:45 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
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
	int	len;
	int	i;

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

int	env_unset_var(char ***penvp, const char *name)
{
	int	idx;
	int	i;

	if (!env_identifier_valid(name))
		return (0);
	idx = env_find_index(*penvp, name);
	if (idx < 0)
		return (1);
	free((*penvp)[idx]);
	i = idx;
	while ((*penvp)[i])
	{
		(*penvp)[i] = (*penvp)[i + 1];
		i++;
	}
	return (1);
}

/* moved helpers and setters to env_set.c; env copy helpers to env_copy.c */
