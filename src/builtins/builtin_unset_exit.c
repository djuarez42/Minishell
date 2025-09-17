/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_exit.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/17 17:08:58 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "libft.h"

int	bi_unset(char **argv, char ***penvp)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (env_identifier_valid(argv[i]))
			(void)env_unset_var(penvp, argv[i]);
		i++;
	}
	return (0);
}

static int	is_valid_number(const char *s)
{
	int	i;
	int	digits;

	if (!s || !*s)
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	digits = 0;
	while (s[i])
	{
		if (!ft_isdigit((unsigned char)s[i]))
			return (0);
		digits++;
		i++;
	}
	return (digits > 0);
}

int	bi_exit(char **argv)
{
	long long	code;
	int			argc;

	argc = 0;
	while (argv[argc])
		argc++;
	if (argc == 1)
		return (0);
	if (!is_valid_number(argv[1]))
	{
		fprintf(stderr, "minishell: exit: %s: numeric argument required\n",
			argv[1]);
		return (2);
	}
	if (argc > 2)
	{
		fprintf(stderr, "minishell: exit: too many arguments\n");
		return (1);
	}
	code = (long long)ft_atoi(argv[1]);
	return ((int)(code & 0xFF));
}
