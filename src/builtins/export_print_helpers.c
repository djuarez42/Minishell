/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print_helpers.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 22:05:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/06 13:39:03 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*export_dup_name(const char *env)
{
	const char	*eq;
	size_t		len;

	eq = ft_strchr(env, '=');
	if (!eq)
		return (ft_strdup(env));
	len = (size_t)(eq - env);
	return (ft_strndup(env, len));
}

int	export_cmp_env(const void *a, const void *b)
{
	const char	*sa;
	const char	*sb;
	char		*a_name;
	char		*b_name;
	int			res;

	sa = *(const char *const *)a;
	sb = *(const char *const *)b;
	if (!sa || !sb)
	{
		if (sa == sb)
			return (0);
		if (sa)
			return (1);
		return (-1);
	}
	a_name = export_dup_name(sa);
	b_name = export_dup_name(sb);
	res = ft_strcmp(a_name, b_name);
	free(a_name);
	free(b_name);
	return (res);
}

char	**export_copy_env(char **envp, int *out_n)
{
	int		n;
	int		i;
	char	**copy;

	n = 0;
	while (envp && envp[n])
		n++;
	*out_n = n;
	copy = (char **)malloc(sizeof(char *) * (n + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < n)
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[n] = NULL;
	return (copy);
}

static size_t	export_count_extra(const char *val)
{
	size_t	i;
	size_t	extra;

	if (!val)
		return (0);
	extra = 0;
	i = 0;
	while (val[i])
	{
		if (val[i] == '"' || val[i] == '\\')
			extra++;
		i++;
	}
	return (extra);
}

char	*export_quote_value_double(const char *val)
{
	size_t	extra;
	char	*out;
	size_t	pos;
	size_t	len;
	size_t	i;

	if (!val)
		return (ft_strdup(""));
	extra = export_count_extra(val);
	len = ft_strlen(val);
	out = malloc(len + extra + 3);
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
