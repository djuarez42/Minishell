/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 19:53:41 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/12 22:20:57 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

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

int	env_set_var(char ***penvp, const char *name, const char *value)
{
	int	idx;
	int	count;
	char	*entry;

	if (!env_identifier_valid(name))
		return (0);
	entry = NULL;
	if (value)
	{
		entry = ft_strjoin(name, "=");
		entry = ft_strjoin_free(entry, (char *)value);
	}
	else
		entry = ft_strjoin(name, "=");
	if (!entry)
		return (0);
	idx = env_find_index(*penvp, name);
	if (idx < 0)
	{
		// insert new at the beginning to match expected ordering in tests
		count = 0;
		while ((*penvp)[count])
			count++;
		if (!realloc_env(penvp, count + 1))
			return (free(entry), 0);
		// shift existing pointers right by one position
		while (count > 0)
		{
			(*penvp)[count] = (*penvp)[count - 1];
			count--;
		}
		(*penvp)[0] = entry;
		return (1);
	}
	// replace existing
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
	{
		// name only: do not create empty entry here; validate identifier and return success
		return env_identifier_valid(assignment);
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
	int idx;
	int i;

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
