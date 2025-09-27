/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:07:21 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 18:52:04 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	print_export_ident_error(const char *s)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	write(STDERR_FILENO, s, ft_strlen(s));
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
}

int	assign_or_error(char ***penvp, const char *arg)
{
	if (env_set_assignment(penvp, arg) != 0)
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

char	*get_name_from_plus_equal(const char *arg)
{
	char	*plus;

	plus = ft_strchr(arg, '+');
	if (!plus)
		return (NULL);
	return (ft_strndup(arg, plus - arg));
}
