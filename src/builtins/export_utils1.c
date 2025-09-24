/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:46:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 17:00:31 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*quote_value_single(const char *val)
{
	size_t	extra;

	if (!val)
		return (ft_strdup(""));
	if (!needs_quotes(val))
		return (ft_strdup(val));
	extra = calc_extra_quotes(val);
	return (build_quoted_value(val, extra));
}

static void	handle_export_value(char *name, char *val)
{
	char	*q;

	if (ft_strlen(name) == 1 && name[0] == '_')
		return ;
	q = quote_value_single(val);
	if (q)
	{
		printf("declare -x %s=%s\n", name, q);
		free(q);
	}
	else
	{
		printf("declare -x %s=%s\n", name, val);
	}
}

void	print_env_entry(char *entry)
{
	char	*eq;
	char	*name;
	char	*val;
	size_t	name_len;

	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		printf("declare -x %s\n", entry);
		return ;
	}
	name_len = eq - entry;
	if (*(eq + 1) == '\0')
	{
		printf("declare -x %.*s=\"\"\n", (int)name_len, entry);
		return ;
	}
	name = ft_strndup(entry, name_len);
	if (!name)
		return ;
	val = eq + 1;
	handle_export_value(name, val);
	free(name);
}

int	count_env_entries(char **envp)
{
	int	n;

	n = 0;
	while (envp[n])
		n++;
	return (n);
}

char	**copy_env_entries(char **envp, int n)
{
	int		i;
	char	**copy;

	copy = malloc(sizeof(char *) * (n + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < n)
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
	copy[n] = NULL;
	return (copy);
}
