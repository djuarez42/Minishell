/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils16.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 19:53:41 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 20:27:06 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_set_assignment(char ***penvp, const char *assignment)
{
	char	*eq;
	char	*name;
	int		ok;

	if (!assignment)
		return (0);
	eq = ft_strchr((char *)assignment, '=');
	if (!eq)
	{
		return (env_identifier_valid(assignment));
	}
	name = ft_substr(assignment, 0, eq - assignment);
	if (!name)
		return (0);
	ok = env_set_var(penvp, name, eq + 1);
	free(name);
	return (ok);
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

char	**new_envp(char **envp)
{
	int		i;
	int		count;
	char	**copy;

	count = 0;
	while (envp[count])
		count++;
	copy = malloc(sizeof(char *) * (count +1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]);
		if (!copy[i])
		{
			while (i > 0)
				free(copy[--i]);
			free(copy);
			return (NULL);
		}
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

void	free_envp(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
		free(envp[i++]);
	free(envp);
}
