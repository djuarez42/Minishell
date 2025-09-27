/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_exit.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 21:23:39 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	bi_unset(char **argv, char ***penvp)
{
	int		i;
	int		status;

	i = 1;
	status = 0;
	if (argv[i] && argv[i][0] == '-')
		return (unset_print_invalid_option(argv[i]));
	while (argv[i])
	{
		if (env_identifier_valid(argv[i]))
			(void)env_unset_var(penvp, argv[i]);
		i++;
	}
	return (status);
}

int	bi_exit(char **argv)
{
	long long		code;
	int				argc;

	argc = 0;
	while (argv[argc])
		argc++;
	if (argc == 1)
		return (0);
	if (!exit_is_valid_number(argv[1]))
		return (exit_print_num_args(argv[1]));
	if (argc > 2)
		return (exit_print_many_args());
	code = (long long)ft_atoi(argv[1]);
	return ((int)(code & 0xFF));
}
