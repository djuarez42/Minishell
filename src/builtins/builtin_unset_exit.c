/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_exit.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/25 17:18:35 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static int	print_num_args(const char *arg)
{
	char	errbuf[512];
	size_t	len;

	len = 0;
	if (arg)
	{
		while (arg[len] && len < sizeof(errbuf) - 32)
		{
			errbuf[len] = arg[len];
			len++;
		}
		errbuf[len] = '\0';
	}
	else
		errbuf[0] = '\0';
	ft_strlcat(errbuf, ": numeric argument required", sizeof(errbuf));
	print_error("exit", errbuf);
	return (2);
}

static int	print_many_args(void)
{
	print_error("exit", "too many arguments");
	return (1);
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
		return (print_num_args(argv[1]));
	if (argc > 2)
		return (print_many_args());
	code = (long long)ft_atoi(argv[1]);
	return ((int)(code & 0xFF));
}

/*static int	is_valid_number(const char *s)
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
		exit(0);
	if (!is_valid_number(argv[1]))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		exit(255);
	}
	if (argc > 2)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		return (1);
	}
	code = (long long)ft_atoi(argv[1]);
	exit((unsigned char)code);
}*/
