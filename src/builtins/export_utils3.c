/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:05:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 17:05:32 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*dup_name(const char *env)
{
	const char	*eq;
	size_t		len;

	eq = ft_strchr(env, '=');
	if (!eq)
		return (ft_strdup(env));
	len = eq - env;
	return (ft_strndup(env, len));
}

int	cmp_env(const void *a, const void *b)
{
	const char	*sa;
	const char	*sb;
	char		*a_name;
	char		*b_name;
	int			res;

	sa = *(const char **)a;
	sb = *(const char **)b;
	if (!sa && !sb)
		return (0);
	if (!sa)
		return (-1);
	if (!sb)
		return (1);
	a_name = dup_name(sa);
	b_name = dup_name(sb);
	res = ft_strcmp(a_name, b_name);
	free(a_name);
	free(b_name);
	return (res);
}

static void	print_export_ident_error(const char *s)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	write(STDERR_FILENO, s, ft_strlen(s));
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
}

int	assign_or_error(char ***penvp, const char *arg)
{
	if (!env_set_assignment(penvp, arg))
	{
		print_export_ident_error(arg);
		return (1);
	}
	return (0);
}

int	validate_name_or_error(const char *arg)
{
	if (!env_identifier_valid(arg))
	{
		print_export_ident_error(arg);
		return (1);
	}
	return (0);
}

static char	*get_name_from_plus_equal(const char *arg)
{
	char	*plus;

	plus = ft_strchr(arg, '+');
	if (!plus)
		return (NULL);
	return (ft_strndup(arg, plus - arg));
}

static int	validate_and_get_name(const char *arg, char **out_name)
{
	*out_name = get_name_from_plus_equal(arg);
	if (!*out_name || !env_identifier_valid(*out_name))
	{
		free(*out_name);
		print_export_ident_error(arg);
		return (0);
	}
	return (1);
}

static char	*get_value_part(const char *arg)
{
	char	*val;

	val = ft_strdup(ft_strchr(arg, '=') + 1);
	return (val);
}

static char	*get_old_value(char **envp, const char *name)
{
	int	idx;

	idx = env_find_index(envp, name);
	if (idx >= 0)
		return (ft_strdup(ft_strchr(envp[idx], '=') + 1));
	return (ft_strdup(""));
}

static int	append_and_set(char ***penvp, char *name,
			char *old_val, char *new_val)
{
	char	*joined;

	joined = ft_strjoin(old_val, new_val);
	free(old_val);
	free(new_val);
	if (!joined)
		return (free(name), 1);
	env_set_var(penvp, name, joined);
	free(name);
	free(joined);
	return (0);
}

int	env_append_assignment(char ***penvp, const char *arg)
{
	char	*name;
	char	*value;
	char	*old_value;

	if (!validate_and_get_name(arg, &name))
		return (1);
	value = get_value_part(arg);
	if (!value)
		return (free(name), 1);
	old_value = get_old_value(*penvp, name);
	if (!old_value)
		return (free(name), free(value), 1);
	return (append_and_set(penvp, name, old_value, value));
}
