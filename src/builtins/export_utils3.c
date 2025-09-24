/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:05:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 17:15:30 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	validate_and_get_name(const char *arg, char **out_name)
{
	*out_name = get_name_from_plus_equal(arg);
	if (!*out_name || !env_identifier_valid(*out_name))
	{
		free(*out_name);
		print_export_ident_error(arg);
		return (0);
	}
	return (1);
}

static char	*get_value_part(const char *arg)
{
	char	*val;

	val = ft_strdup(ft_strchr(arg, '=') + 1);
	return (val);
}

static char	*get_old_value(char **envp, const char *name)
{
	int	idx;

	idx = env_find_index(envp, name);
	if (idx >= 0)
		return (ft_strdup(ft_strchr(envp[idx], '=') + 1));
	return (ft_strdup(""));
}

static int	append_and_set(char ***penvp, char *name,
			char *old_val, char *new_val)
{
	char	*joined;

	joined = ft_strjoin(old_val, new_val);
	free(old_val);
	free(new_val);
	if (!joined)
		return (free(name), 1);
	env_set_var(penvp, name, joined);
	free(name);
	free(joined);
	return (0);
}

int	env_append_assignment(char ***penvp, const char *arg)
{
	char	*name;
	char	*value;
	char	*old_value;

	if (!validate_and_get_name(arg, &name))
		return (1);
	value = get_value_part(arg);
	if (!value)
		return (free(name), 1);
	old_value = get_old_value(*penvp, name);
	if (!old_value)
		return (free(name), free(value), 1);
	return (append_and_set(penvp, name, old_value, value));
}
