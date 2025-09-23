/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/23 20:39:04 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libft.h"
#include "executor.h"
#include "builtins.h"

/* ------------------------ AUXILIARY FUNCTIONS ------------------------ */

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
	const char	*sa = *(const char **)a;
	const char	*sb = *(const char **)b;

	if (!sa && !sb)
		return (0);
	if (!sa)
		return (-1);
	if (!sb)
		return (1);

	char *a_name = dup_name(sa);
	char *b_name = dup_name(sb);
	int res = strcmp(a_name, b_name);
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

/* ------------------------ ENV MANAGEMENT ------------------------ */

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
	char *plus = ft_strchr(arg, '+');
	if (!plus)
		return (NULL);
	return (ft_strndup(arg, plus - arg));
}

static int	env_append_assignment(char ***penvp, const char *arg)
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
}

/* ------------------------ PRINT ENV ------------------------ */

static int	needs_quotes(const char *val)
{
	size_t i = 0;

	if (!val)
		return (0);
	while (val[i])
	{
		if (!(ft_isalnum((unsigned char)val[i]) || val[i] == '_' || val[i] == '/' ||
			  val[i] == '.' || val[i] == ':' || val[i] == ',' || val[i] == '-' || val[i] == '@'))
			return (1);
		i++;
	}
	return (0);
}

static char	*quote_value_single(const char *val)
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
}

static void	print_exported_env(char **envp)
{
	int		i, n;
	char	**copy;

	if (!envp)
		return;

	/* contar */
	n = 0;
	while (envp[n])
		n++;

	/* copiar */
	copy = malloc(sizeof(char *) * (n + 1));
	if (!copy)
		return;
	i = 0;
	while (i < n)
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[n] = NULL;

	/* ordenar */
	qsort(copy, n, sizeof(char *), cmp_env);

	/* imprimir */
	i = 0;
	while (i < n)
	{
		char *eq = ft_strchr(copy[i], '=');
		if (!eq) // caso: export a → solo nombre
		{
			printf("declare -x %s\n", copy[i]);
		}
		else if (*(eq + 1) == '\0') // caso: export a= → cadena vacía
		{
			size_t name_len = eq - copy[i];
			printf("declare -x %.*s=\"\"\n", (int)name_len, copy[i]);
		}
		else // caso: export a=valor
		{
			size_t name_len = eq - copy[i];
			char *name = ft_strndup(copy[i], name_len);
			char *val = eq + 1;
			if (!(ft_strlen(name) == 1 && name[0] == '_')) // ignorar "_"
			{
				char *q = quote_value_single(val);
				printf("declare -x %s=%s\n", name, q ? q : val);
				free(q);
			}
			free(name);
		}
		i++;
	}

	/* liberar */
	i = 0;
	while (i < n)
		free(copy[i++]);
	free(copy);
}


/* ------------------------ MAIN BUILTIN FUNCTION ------------------------ */

int	bi_export(char **argv, char ***penvp)
{
	int	i;
	int	status;

	if (!argv[1])
	{
		print_exported_env(*penvp); // si no hay argumentos, imprimimos todo
		return (0);
	}

	status = 0;
	i = 1;
	while (argv[i])
	{
		if (ft_strnstr(argv[i], "+=", ft_strlen(argv[i])))
			status |= env_append_assignment(penvp, argv[i]);
		else if (ft_strchr(argv[i], '='))
			status |= assign_or_error(penvp, argv[i]);
		else
		{
			if (validate_name_or_error(argv[i]) == 0)
				env_set_var(penvp, argv[i], NULL);
			else
				status |= 1;
		}
		i++;
	}
	return (status);
}
