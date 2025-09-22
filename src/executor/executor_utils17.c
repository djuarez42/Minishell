/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils17.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 05:02:16 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 05:15:40 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	realloc_env(char ***penvp, int new_count)
{
	char	**new;
	int		i;

	new = (char **)malloc(sizeof(char *) * (new_count + 1));
	if (!new)
		return (0);
	i = 0;
	while ((*penvp)[i] && i < new_count)
	{
		new[i] = (*penvp)[i];
		i++;
	}
	new[new_count] = NULL;
	free(*penvp);
	*penvp = new;
	return (1);
}

static char	*build_env_entry(const char *name, const char *value)
{
	char	*entry;

	if (value)
	{
		entry = ft_strjoin(name, "=");
		entry = ft_strjoin_free(entry, (char *)value);
	}
	else
		entry = ft_strjoin(name, "=");
	return (entry);
}

static int	insert_new_env_var(char ***penvp, char *entry)
{
	int		count;

	count = 0;
	while ((*penvp)[count])
		count++;
	if (!realloc_env(penvp, count + 1))
	{
		free(entry);
		return (0);
	}
	while (count > 0)
	{
		(*penvp)[count] = (*penvp)[count - 1];
		count--;
	}
	(*penvp)[0] = entry;
	return (1);
}

static int	update_existing_env_var(char ***penvp, int idx, char *entry)
{
	free((*penvp)[idx]);
	(*penvp)[idx] = entry;
	return (1);
}

int	env_set_var(char ***penvp, const char *name, const char *value)
{
	int		idx;
	char	*entry;

	if (!env_identifier_valid(name))
		return (0);
	entry = build_env_entry(name, value);
	if (!entry)
		return (0);
	idx = env_find_index(*penvp, name);
	if (idx < 0)
		return (insert_new_env_var(penvp, entry));
	return (update_existing_env_var(penvp, idx, entry));
}
