/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 15:55:21 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 18:59:58 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* export_print removed — use macro in header to map to printf */

void	export_utils_print_no_value(const char *name)
{
	ft_putstr_fd((char *)"declare -x ", STDOUT_FILENO);
	ft_putstr_fd((char *)name, STDOUT_FILENO);
	ft_putstr_fd((char *)"\n", STDOUT_FILENO);
}

int	export_utils_should_skip_underscore(const char *name)
{
	return (name && name[0] == '_' && name[1] == '\0');
}

static void	print_entry_noeq(const char *line)
{
	export_utils_print_no_value(line);
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
	qsort(copy, n, sizeof (*copy), export_utils_cmp_env);
	print_exported_entries(copy);
	free(copy);
}
