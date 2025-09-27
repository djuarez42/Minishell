/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:26:24 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	print_invalid_option(const char *arg)
{
	char	opt[3];

	opt[0] = '-';
	opt[1] = 0;
	opt[2] = 0;
	if (arg && arg[0] == '-' && arg[1])
		opt[1] = arg[1];
	else
		opt[1] = '?';
	ft_putstr_fd("minishell: export: ", STDERR_FILENO);
	ft_putstr_fd(opt, STDERR_FILENO);
	ft_putendl_fd(": invalid option", STDERR_FILENO);
	return (2);
}

static int	is_invalid_option_token(const char *arg)
{
	if (!arg || arg[0] != '-')
		return (0);
	if (arg[1] == '\0')
		return (1);
	if (arg[1] == '-' && arg[2] == '\0')
		return (0);
	return (1);
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
	if (is_invalid_option_token(argv[i]))
		return (print_invalid_option(argv[i]));
	if (argv[i] && argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == '\0')
		i++;
	while (argv[i])
	{
		status |= handle_export_arg(penvp, argv[i]);
		i++;
	}
	return (status);
}
