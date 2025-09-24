/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/24 16:46:35 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**duplicate_env(char **envp, int *out_size)
{
	int		n;
	char	**copy;

	if (!envp)
		return (NULL);
	n = count_env_entries(envp);
	copy = copy_env_entries(envp, n);
	if (!copy)
		return (NULL);
	if (out_size)
		*out_size = n;
	return (copy);
}

static void	sort_env_array(char **arr, int n)
{
	int		i;
	int		j;
	char	*tmp;

	if (!arr || n <= 1)
		return ;
	i = 0;
	while (i < n - 1)
	{
		j = i + 1;
		while (j < n)
		{
			if (cmp_env(&arr[i], &arr[j]) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static void	print_exported_env(char **envp)
{
	int		i;
	int		n;
	char	**copy;

	if (!envp)
		return ;
	copy = duplicate_env(envp, &n);
	if (!copy)
		return ;
	sort_env_array(copy, n);
	i = 0;
	while (i < n)
	{
		print_env_entry(copy[i]);
		i++;
	}
	i = 0;
	while (i < n)
		free(copy[i++]);
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
