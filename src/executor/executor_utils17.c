/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils17.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 05:02:16 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 19:45:12 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	realloc_env(char ***penvp, int new_count)
{
	char	**new;
	int		i;

	new = (char **)malloc(sizeof(char *) * (new_count + 1));
	if (!new)
		return (1);
	i = 0;
	while ((*penvp)[i] && i < new_count)
	{
		new[i] = (*penvp)[i];
		i++;
	}
	new[new_count] = NULL;
	free(*penvp);
	*penvp = new;
	return (0);
}

static char	*build_env_entry(const char *name, const char *value)
{
	char	*entry;

	if (!name)
		return (NULL);
	if (value == NULL)
		return (ft_strdup(name));
	entry = ft_strjoin(name, "=");
	if (!entry)
		return (NULL);
	entry = ft_strjoin_free(entry, (char *)value);
	return (entry);
}

static int	insert_new_env_var(char ***penvp, char *entry)
{
	int	count;

	count = 0;
	while ((*penvp)[count])
		count++;
	if (realloc_env(penvp, count + 1) != 0)
	{
		free(entry);
		return (1);
	}
	while (count > 0)
	{
		(*penvp)[count] = (*penvp)[count - 1];
		count--;
	}
	(*penvp)[0] = entry;
	return (0);
}

int	update_env(char ***envp_copy, char *new_var)
{
	int		i;
	char	*name;
	int		len;

	len = 0;
	while (new_var[len] && new_var[len] != '=')
		len++;
	name = ft_substr(new_var, 0, len);
	if (!name)
		return (1);
	i = 0;
	while ((*envp_copy)[i])
	{
		if (ft_strncmp((*envp_copy)[i], name, len) == 0
			&& ((*envp_copy)[i][len] == '=' || (*envp_copy)[i][len] == '\0'))
		{
			free((*envp_copy)[i]);
			(*envp_copy)[i] = ft_strdup(new_var);
			free(name);
			return (0);
		}
		i++;
	}
	free(name);
	return (1);
}

int	env_set_var(char ***penvp, const char *name, const char *value)
{
	char	*entry;
	int		res;

	if (!env_identifier_valid(name))
		return (1);
	entry = build_env_entry(name, value);
	if (!entry)
		return (1);
	if (update_env(penvp, entry) == 1)
	{
		res = insert_new_env_var(penvp, entry);
		return (res);
	}
	free(entry);
	return (0);
}
