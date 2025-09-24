/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/24 16:08:17 by djuarez          ###   ########.fr       */
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

static int	cmp_env(const void *a, const void *b)
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

static int	assign_or_error(char ***penvp, const char *arg)
{
	if (!env_set_assignment(penvp, arg))
	{
		print_export_ident_error(arg);
		return (1);
	}
	return (0);
}

static int	validate_name_or_error(const char *arg)
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

static int	env_append_assignment(char ***penvp, const char *arg)
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

/*static int	env_append_assignment(char ***penvp, const char *arg)
{
	char	*name;
	char	*value;
	char	*old_value;
	char	*new_value;
	int		idx;

	name = get_name_from_plus_equal(arg);
	if (!name || !env_identifier_valid(name))
	{
		free(name);
		print_export_ident_error(arg);
		return (1);
	}
	value = ft_strdup(ft_strchr(arg, '=') + 1);
	if (!value)
		return (free(name), 1);
	idx = env_find_index(*penvp, name);
	if (idx >= 0)
		old_value = ft_strdup(ft_strchr((*penvp)[idx], '=') + 1);
	else
		old_value = ft_strdup("");
	if (!old_value)
		return (free(name), free(value), 1);
	new_value = ft_strjoin(old_value, value);
	free(old_value);
	free(value);
	if (!new_value)
		return (free(name), 1);
	env_set_var(penvp, name, new_value);
	free(name);
	free(new_value);
	return (0);
}*/

static int	needs_quotes(const char *val)
{
	size_t	i;

	if (!val)
		return (0);
	i = 0;
	while (val[i])
	{
		if (!(ft_isalnum((unsigned char)val[i])
				|| val[i] == '_'
				|| val[i] == '/'
				|| val[i] == '.'
				|| val[i] == ':'
				|| val[i] == ','
				|| val[i] == '-'
				|| val[i] == '@'))
			return (1);
		i++;
	}
	return (0);
}

static size_t	calc_extra_quotes(const char *val)
{
	size_t	i;
	size_t	extra;

	i = 0;
	extra = 0;
	while (val[i])
	{
		if (val[i] == '\'')
			extra += 3;
		i++;
	}
	return (extra);
}

static char	*alloc_quoted_buffer(const char *val, size_t extra)
{
	size_t	len;
	char	*out;

	len = ft_strlen(val);
	out = malloc(len + extra + 3);
	return (out);
}

static void	append_escaped_quote(char *out, size_t *pos)
{
	out[(*pos)++] = '\'';
	out[(*pos)++] = '\\';
	out[(*pos)++] = '\'';
	out[(*pos)++] = '\'';
}

static char	*build_quoted_value(const char *val, size_t extra)
{
	size_t	i;
	size_t	pos;
	char	*out;

	out = alloc_quoted_buffer(val, extra);
	if (!out)
		return (NULL);
	pos = 0;
	out[pos++] = '\'';
	i = 0;
	while (val[i])
	{
		if (val[i] == '\'')
			append_escaped_quote(out, &pos);
		else
			out[pos++] = val[i];
		i++;
	}
	out[pos++] = '\'';
	out[pos] = '\0';
	return (out);
}

static char	*quote_value_single(const char *val)
{
	size_t	extra;

	if (!val)
		return (ft_strdup(""));
	if (!needs_quotes(val))
		return (ft_strdup(val));
	extra = calc_extra_quotes(val);
	return (build_quoted_value(val, extra));
}

/*static char	*quote_value_single(const char *val)
{
	size_t i, extra = 0, pos;
	char *out;

	if (!val)
		return (ft_strdup(""));
	if (!needs_quotes(val))
		return (ft_strdup(val));
	i = 0;
	while (val[i])
	{
		if (val[i] == '\'')
			extra += 3;
		i++;
	}
	out = malloc(ft_strlen(val) + extra + 3);
	if (!out)
		return (NULL);
	pos = 0;
	out[pos++] = '\'';
	i = 0;
	while (val[i])
	{
		if (val[i] == '\'')
		{
			out[pos++] = '\'';
			out[pos++] = '\\';
			out[pos++] = '\'';
			out[pos++] = '\'';
		}
		else
			out[pos++] = val[i];
		i++;
	}
	out[pos++] = '\'';
	out[pos] = '\0';
	return (out);
}*/

/*static void	print_exported_env(char **envp)
{
	int		i, n;
	char	**copy;

	if (!envp)
		return ;
	n = 0;
	while (envp[n])
		n++;
	copy = malloc(sizeof(char *) * (n + 1));
	if (!copy)
		return ;
	i = 0;
	while (i < n)
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[n] = NULL;
	qsort(copy, n, sizeof(char *), cmp_env);
	i = 0;
	while (i < n)
	{
		char *eq = ft_strchr(copy[i], '=');
		if (!eq)
		{
			printf("declare -x %s\n", copy[i]);
		}
		else if (*(eq + 1) == '\0')
		{
			size_t name_len = eq - copy[i];
			printf("declare -x %.*s=\"\"\n", (int)name_len, copy[i]);
		}
		else
		{
			size_t name_len = eq - copy[i];
			char *name = ft_strndup(copy[i], name_len);
			char *val = eq + 1;
			if (!(ft_strlen(name) == 1 && name[0] == '_'))
			{
				char *q = quote_value_single(val);
				printf("declare -x %s=%s\n", name, q ? q : val);
				free(q);
			}
			free(name);
		}
		i++;
	}
	i = 0;
	while (i < n)
		free(copy[i++]);
	free(copy);
}*/

/*static char	**duplicate_env(char **envp, int *out_size)
{
	int		i;
	int		n;
	char	**copy;

	if (!envp)
		return (NULL);
	n = 0;
	while (envp[n])
		n++;
	copy = malloc(sizeof(char *) * (n + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < n)
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[n] = NULL;
	if (out_size)
		*out_size = n;
	return (copy);
}*/

static void	handle_export_value(char *name, char *val)
{
	char	*q;

	if (ft_strlen(name) == 1 && name[0] == '_')
		return ;
	q = quote_value_single(val);
	if (q)
	{
		printf("declare -x %s=%s\n", name, q);
		free(q);
	}
	else
	{
		printf("declare -x %s=%s\n", name, val);
	}
}

static void	print_env_entry(char *entry)
{
	char	*eq;
	char	*name;
	char	*val;
	size_t	name_len;

	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		printf("declare -x %s\n", entry);
		return ;
	}
	name_len = eq - entry;
	if (*(eq + 1) == '\0')
	{
		printf("declare -x %.*s=\"\"\n", (int)name_len, entry);
		return ;
	}
	name = ft_strndup(entry, name_len);
	if (!name)
		return ;
	val = eq + 1;
	handle_export_value(name, val);
	free(name);
}

/*static void	print_env_entry(char *entry)
{
	char	*eq;
	char	*name;
	char	*val;
	char	*q;
	size_t	name_len;

	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		printf("declare -x %s\n", entry);
		return ;
	}
	name_len = eq - entry;
	if (*(eq + 1) == '\0')
	{
		printf("declare -x %.*s=\"\"\n", (int)name_len, entry);
		return ;
	}
	name = ft_strndup(entry, name_len);
	if (!name)
		return ;
	val = eq + 1;
	if (ft_strlen(name) != 1 || name[0] != '_')
	{
		q = quote_value_single(val);
		if (q)
		{
			printf("declare -x %s=%s\n", name, q);
			free(q);
		}
		else
		{
			printf("declare -x %s=%s\n", name, val);
		}
	}
	free(name);
}*/

/*static void	print_env_entry(char *entry)
{
	char	*eq;
	char	*name;
	char	*val;
	char	*q;
	size_t	name_len;

	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		printf("declare -x %s\n", entry);
		return ;
	}
	name_len = eq - entry;
	if (*(eq + 1) == '\0')
	{
		printf("declare -x %.*s=\"\"\n", (int)name_len, entry);
		return ;
	}
	name = ft_strndup(entry, name_len);
	val = eq + 1;
	if (!(ft_strlen(name) == 1 && name[0] == '_'))
	{
		q = quote_value_single(val);
		printf("declare -x %s=%s\n", name, q ? q : val);
		free(q);
	}
	free(name);
}*/

/*static void	print_exported_env(char **envp)
{
	int		i;
	int		n;
	char	**copy;

	copy = duplicate_env(envp, &n);
	if (!copy)
		return ;
	qsort(copy, n, sizeof(char *), cmp_env);
	i = 0;
	while (i < n)
	{
		print_env_entry(copy[i]);
		i++;
	}
	i = 0;
	while (i < n)
		free(copy[i++]);
	free(copy);
}*/

static char	**duplicate_env(char **envp, int *out_size)
{
	int		i;
	int		n;
	char	**copy;

	if (!envp)
		return (NULL);
	n = 0;
	while (envp[n])
		n++;
	copy = malloc(sizeof(char *) * (n + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < n)
	{
		copy[i] = ft_strdup(envp[i]);
		if (!copy[i])
		{
			while (i > 0)
				free(copy[--i]);
			free(copy);
			return (NULL);
		}
		i++;
	}
	copy[n] = NULL;
	if (out_size)
		*out_size = n;
	return (copy);
}

static void	sort_env_array(char **arr, int n)
{
	int		i;
	int		j;
	char	*tmp;

	if (!arr || n <= 1)
		return ;
	i = 0;
	while (i < n - 1)
	{
		j = i + 1;
		while (j < n)
		{
			if (cmp_env(&arr[i], &arr[j]) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static void	print_exported_env(char **envp)
{
	int		i;
	int		n;
	char	**copy;

	if (!envp)
		return ;
	copy = duplicate_env(envp, &n);
	if (!copy)
		return ;
	sort_env_array(copy, n);
	i = 0;
	while (i < n)
	{
		print_env_entry(copy[i]);
		i++;
	}
	i = 0;
	while (i < n)
		free(copy[i++]);
	free(copy);
}

static int	handle_export_arg(char ***penvp, char *arg)
{
	int	status;

	status = 0;
	if (ft_strnstr(arg, "+=", ft_strlen(arg)))
		status |= env_append_assignment(penvp, arg);
	else if (ft_strchr(arg, '='))
		status |= assign_or_error(penvp, arg);
	else
	{
		if (validate_name_or_error(arg) == 0)
			env_set_var(penvp, arg, NULL);
		else
			status |= 1;
	}
	return (status);
}

int	bi_export(char **argv, char ***penvp)
{
	int	i;
	int	status;

	if (!argv[1])
	{
		print_exported_env(*penvp);
		return (0);
	}
	status = 0;
	i = 1;
	while (argv[i])
	{
		status |= handle_export_arg(penvp, argv[i]);
		i++;
	}
	return (status);
}
