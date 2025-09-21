/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:55:12 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 21:55:12 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static char	*make_entry(const char *name, const char *value)
{
	char	*entry;

	if (value)
	{
		entry = ft_strjoin(name, "=");
		if (!entry)
			return (NULL);
		entry = ft_strjoin_free(entry, (char *)value);
	}
	else
		entry = ft_strjoin(name, "=");
	return (entry);
}

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

static int	insert_new_env(char ***penvp, char *entry)
{
	int	count;

	count = 0;
	while ((*penvp)[count])
		count++;
	if (!realloc_env(penvp, count + 1))
		return (0);
	while (count > 0)
	{
		(*penvp)[count] = (*penvp)[count - 1];
		count--;
	}
	(*penvp)[0] = entry;
	return (1);
}

int	env_set_var(char ***penvp, const char *name, const char *value)
{
	int		idx;
	char	*entry;

	if (!env_identifier_valid(name))
		return (0);
	entry = make_entry(name, value);
	if (!entry)
		return (0);
	idx = env_find_index(*penvp, name);
	if (idx < 0)
		return (insert_new_env(penvp, entry));
	free((*penvp)[idx]);
	(*penvp)[idx] = entry;
	return (1);
}

int	env_set_assignment(char ***penvp, const char *assignment)
{
	char	*eq;
	char	*name;
	int		ok;

	if (!assignment)
		return (0);
	eq = ft_strchr((char *)assignment, '=');
	if (!eq)
		return (env_identifier_valid(assignment));
	name = ft_substr(assignment, 0, eq - assignment);
	if (!name)
		return (0);
	ok = env_set_var(penvp, name, eq + 1);
	free(name);
	return (ok);
}
