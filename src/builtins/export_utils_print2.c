/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_print2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 19:40:40 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:40:43 by ekakhmad         ###   ########.fr       */
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

void	print_exported_env(char **envp)
{
	char	**copy;
	int		n;

	copy = export_utils_copy_env(envp, &n);
	if (!copy)
		return ;
	qsort(copy, n, sizeof(*copy), export_utils_cmp_env);
	print_exported_entries(copy);
	free(copy);
}
