/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_print2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 19:40:40 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/06 13:24:36 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_entry_noeq(const char *line)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd((char *)line, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
}

static void	print_exported_entries(char **copy)
{
	int		i;
	char	*eq;
	char	*name;

	i = 0;
	while (copy[i])
	{
		eq = ft_strchr(copy[i], '=');
		if (!eq)
			print_entry_noeq(copy[i]);
		else
		{
			name = ft_strndup(copy[i], (size_t)(eq - copy[i]));
			if (name && !export_utils_should_skip_underscore(name))
				export_utils_print_with_value(name, eq + 1);
			free(name);
		}
		free(copy[i]);
		++i;
	}
}

static void	sort_env_copy(char **copy, int n)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < n - 1)
	{
		j = 0;
		while (j < n - i - 1)
		{
			if (export_utils_cmp_env(&copy[j], &copy[j + 1]) > 0)
			{
				tmp = copy[j];
				copy[j] = copy[j + 1];
				copy[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	print_exported_env(char **envp)
{
	char	**copy;
	int		n;

	copy = export_utils_copy_env(envp, &n);
	if (!copy)
		return ;
	sort_env_copy(copy, n);
	print_exported_entries(copy);
	free(copy);
}
