/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_print.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 22:21:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:44:08 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// moved helpers: export_utils_quote_value and export_utils_quote_size

static void	export_utils_print_empty_value(const char *name)
{
	ft_putstr_fd((char *)"declare -x ", STDOUT_FILENO);
	ft_putstr_fd((char *)name, STDOUT_FILENO);
	ft_putstr_fd((char *)"=\"\"\n", STDOUT_FILENO);
}

void	export_utils_print_with_value(const char *name, const char *val)
{
	char	*q;

	if (val[0] == '\0')
		return (export_utils_print_empty_value(name));
	q = export_utils_quote_value(val);
	if (q)
	{
		ft_putstr_fd((char *)"declare -x ", STDOUT_FILENO);
		ft_putstr_fd((char *)name, STDOUT_FILENO);
		ft_putstr_fd((char *)"=", STDOUT_FILENO);
		ft_putstr_fd(q, STDOUT_FILENO);
		ft_putstr_fd((char *)"\n", STDOUT_FILENO);
		free(q);
	}
	else
	{
		ft_putstr_fd((char *)"declare -x ", STDOUT_FILENO);
		ft_putstr_fd((char *)name, STDOUT_FILENO);
		ft_putstr_fd((char *)"=", STDOUT_FILENO);
		ft_putstr_fd((char *)val, STDOUT_FILENO);
		ft_putstr_fd((char *)"\n", STDOUT_FILENO);
	}
}

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
