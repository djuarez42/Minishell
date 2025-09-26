/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/26 18:14:50 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	export_sort_entries(char **copy, int n)
{
	int		i;
	int		j;
	char	*tmp;

	if (!copy || n <= 1)
		return ;
	i = 0;
	while (i < n - 1)
	{
		j = i + 1;
		while (j < n)
		{
			if (cmp_env(&copy[i], &copy[j]) > 0)
			{
				tmp = copy[i];
				copy[i] = copy[j];
				copy[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static char	**export_sorted_copy(char **envp, int *out_n)
{
	char	**copy;
	int		n;

	if (!envp)
	{
		*out_n = 0;
		return (NULL);
	}
	copy = export_utils_copy_env(envp, &n);
	if (!copy)
	{
		*out_n = 0;
		return (NULL);
	}
	export_sort_entries(copy, n);
	*out_n = n;
	return (copy);
}

void	print_exported_env(char **envp)
{
	char	**copy;
	int		i;
	int		n;

	if (!envp)
		return ;
	copy = export_sorted_copy(envp, &n);
	if (!copy)
		return ;
	i = 0;
	while (i < n)
	{
		print_env_entry(copy[i]);
		i++;
	}
	i = 0;
	while (i < n)
	{
		free(copy[i]);
		i++;
	}
	free(copy);
}

static int	handle_export_arg(char ***penvp, char *arg)
{
	int	status;

	status = 0;
	if (ft_strnstr(arg, "+=", ft_strlen(arg)))
		status |= env_append_assignment(penvp, arg);
	else if (ft_strchr(arg, '='))
		status |= assign_or_error(penvp, arg);
	else
	{
		if (validate_name_or_error(arg) == 0)
			env_set_var(penvp, arg, NULL);
		else
			status |= 1;
	}
	return (status);
}

int	bi_export(char **argv, char ***penvp)
{
	int	i;
	int	status;

	if (!argv[1])
	{
		print_exported_env(*penvp);
		return (0);
	}
	status = 0;
	i = 1;
	while (argv[i])
	{
		status |= handle_export_arg(penvp, argv[i]);
		i++;
	}
	return (status);
}
