/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/25 17:29:55 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/* 
static char *dup_name(const char *env)
{
	const char *eq;
	size_t len;

	eq = ft_strchr(env, '=');
	if (!eq)
		return (ft_strdup(env));
	len = eq - env;
	return (ft_strndup(env, len));
}
static int cmp_env(const void *a, const void *b)
{
	const char *sa = *(const char **)a;
	const char *sb = *(const char **)b;

	if (!sa && !sb)
		return (0);
	if (!sa)
		return (-1);
	if (!sb)
		return (1);
	{
		char *a_name = dup_name(sa);
		char *b_name = dup_name(sb);
		int res = strcmp(a_name, b_name);
		free(a_name);
		free(b_name);
		return (res);
	}
}
 */
/* 
static int needs_single_quotes(const char *s)
{
	size_t i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
	if (!(ft_isalnum((unsigned char)s[i]) || s[i] == '_' || s[i] == '/' 
		|| s[i] == '.' || s[i] == ':' || s[i] == ',' || s[i] == '-' || 
		s[i] == '@'))
			return (1);
		i++;
	}
	return (0);
}
 */
/* static char *quote_value_single(const char *val) 
{
	size_t i;
	size_t extra = 0;
	char *out;
	size_t pos;

	if (!val)
		return (ft_strdup(""));
	if (!needs_single_quotes(val))
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
*/
static void	print_export_ident_error(const char *s)
{
	char	buf[512];

	buf[0] = '\0';
	ft_strlcat(buf, "`", sizeof(buf));
	if (s)
		ft_strlcat(buf, s, sizeof(buf));
	ft_strlcat(buf, "': not a valid identifier", sizeof(buf));
	print_error("export", buf);
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
/* 
void	print_exported_env(char **envp)
{
	int i;
	int n;
	char **copy;

	if (!envp)
		return;

	n = 0;
	while (envp[n])
		n++;
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
	
	qsort(copy, n, sizeof(char *), cmp_env);
	i = 0;
	while (copy[i])
	{
		char *eq = ft_strchr(copy[i], '=');
		if (!eq)
			   ft_putstr_fd(copy[i], 1);
			   ft_putendl_fd("", 1);
		else
		{
			size_t name_len = eq - copy[i];
			char *name = ft_strndup(copy[i], name_len);
			char *val = eq + 1;
		
			if (ft_strlen(name) == 1 && name[0] == '_')
			{
				free(name);
				free(copy[i]);
				i++;
				continue;
			}
			
			if (val && val[0] == '\0')
			{
				   ft_putstr_fd(name, 1);
				   ft_putstr_fd("=''", 1);
				   ft_putendl_fd("", 1);
				free(name);
				free(copy[i]);
				i++;
				continue;
			}
			char *q = quote_value_single(val);
			if (q)
			{
				   ft_putstr_fd(name, 1);
				   ft_putstr_fd("=", 1);
				   ft_putstr_fd(q, 1);
				   ft_putendl_fd("", 1);
				free(q);
			}
			else
				   ft_putstr_fd(name, 1);
				   ft_putstr_fd("=", 1);
				   ft_putstr_fd(val, 1);
				   ft_putendl_fd("", 1);
			free(name);
		}
		free(copy[i]);
		i++;
	}
	free(copy);
}
 */

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
		if (ft_strchr(argv[i], '='))
			status |= assign_or_error(penvp, argv[i]);
		else
			status |= validate_name_or_error(argv[i]);
		i++;
	}
	return (status);
}
