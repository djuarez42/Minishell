/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_print_helpers.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 19:43:21 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:44:07 by ekakhmad         ###   ########.fr       */
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
