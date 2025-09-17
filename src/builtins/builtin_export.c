/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/17 17:19:00 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libft.h"
#include "executor.h"
#include "builtins.h"

static void	print_export_ident_error(const char *s)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	write(STDERR_FILENO, s, ft_strlen(s));
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
}

static int	assign_or_error(char ***penvp, const char *arg)
{
	if (!env_set_assignment(penvp, arg))
	{
		print_export_ident_error(arg);
		return (1);
	}
	return (0);
}

static int	validate_name_or_error(const char *arg)
{
	if (!env_identifier_valid(arg))
	{
		print_export_ident_error(arg);
		return (1);
	}
	return (0);
}

static void	print_exported_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		printf("declare -x %s\n", envp[i]);
		i++;
	}
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
		if (ft_strchr(argv[i], '='))
			status |= assign_or_error(penvp, argv[i]);
		else
			status |= validate_name_or_error(argv[i]);
		i++;
	}
	return (status);
}
