/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_names.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 22:20:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/10/06 13:40:12 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	export_utils_count_env(char **envp)
{
	int	n;

	n = 0;
	while (envp && envp[n])
		n++;
	return (n);
}

char	*export_utils_dup_name(const char *env)
{
	const char	*eq;
	size_t		len;

	eq = ft_strchr(env, '=');
	if (!eq)
		return (ft_strdup(env));
	len = (size_t)(eq - env);
	return (ft_strndup(env, len));
}

int	export_utils_cmp_env(const void *a, const void *b)
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
	a_name = export_utils_dup_name(sa);
	b_name = export_utils_dup_name(sb);
	res = ft_strcmp(a_name, b_name);
	free(a_name);
	free(b_name);
	return (res);
}

char	**export_utils_copy_env(char **envp, int *out_n)
{
	int		n;
	int		i;
	char	**copy;

	n = export_utils_count_env(envp);
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
