/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_exit_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 21:05:27 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 21:16:16 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset_print_invalid_option(const char *arg)
{
	char	opt[3];

	opt[0] = '-';
	opt[1] = 0;
	opt[2] = 0;
	if (arg && arg[0] == '-' && arg[1])
		opt[1] = arg[1];
	else
		opt[1] = '?';
	ft_putstr_fd("minishell: unset: ", STDERR_FILENO);
	ft_putstr_fd(opt, STDERR_FILENO);
	ft_putendl_fd(": invalid option", STDERR_FILENO);
	return (2);
}

int	exit_is_valid_number(const char *s)
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

int	exit_print_num_args(const char *arg)
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

int	exit_print_many_args(void)
{
	print_error("exit", "too many arguments");
	return (1);
}
