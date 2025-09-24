/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_print.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 22:21:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/23 22:20:36 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	export_utils_quote_size(const char *val)
{
	size_t	i;
	size_t	extra;

	if (!val)
		return (2);
	extra = 0;
	i = 0;
	while (val[i])
	{
		if (val[i] == '"' || val[i] == '\\')
			extra++;
		i++;
	}
	return (ft_strlen(val) + extra + 3);
}

char	*export_utils_quote_value(const char *val)
{
	char	*out;
	size_t	i;
	size_t	pos;

	if (!val)
		return (ft_strdup(""));
	out = malloc(export_utils_quote_size(val));
	if (!out)
		return (NULL);
	pos = 0;
	out[pos++] = '"';
	i = 0;
	while (val[i])
	{
		if (val[i] == '"' || val[i] == '\\')
			out[pos++] = '\\';
		out[pos++] = val[i++];
	}
	out[pos++] = '"';
	out[pos] = '\0';
	return (out);
}

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
