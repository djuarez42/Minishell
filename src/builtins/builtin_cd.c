/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 19:32:24 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_pwd_vars(char ***penvp)
{
	char	*oldpwd;
	char	*newpwd;
	char	*env_old;

	env_old = env_get_value(*penvp, "PWD");
	if (env_old)
		oldpwd = ft_strdup(env_old);
	else
		oldpwd = getcwd(NULL, 0);
	newpwd = getcwd(NULL, 0);
	if (oldpwd)
		env_set_var(penvp, "OLDPWD", oldpwd);
	if (newpwd)
		env_set_var(penvp, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
}

static const char	*resolve_cd_target(char **argv, char ***penvp, int *type)
{
	const char	*path;
	char		*temp;

	if (!argv[1] || (argv[1] && ft_strcmp(argv[1], "--") == 0))
	{
		*type = 0;
		return (env_get_value(*penvp, "HOME"));
	}
	if (ft_strncmp(argv[1], "-", 2) == 0)
	{
		*type = 1;
		path = env_get_value(*penvp, "OLDPWD");
		if (path)
		{
			temp = ft_strdup(path);
			if (temp)
			{
				ft_putendl_fd(temp, STDOUT_FILENO);
				free(temp);
			}
		}
		return (path);
	}
	*type = -1;
	return (argv[1]);
}

static int	change_dir_and_update(const char *path, char ***penvp, int type)
{
	if (!path)
	{
		if (type == 0)
			ft_putendl_fd("cd: HOME not set", STDERR_FILENO);
		else if (type == 1)
			ft_putendl_fd("cd: OLDPWD not set", STDERR_FILENO);
		return (1);
	}
	if (chdir(path) == -1)
	{
		print_error("cd", strerror(errno));
		return (1);
	}
	update_pwd_vars(penvp);
	return (0);
}

int	bi_cd(char **argv, char ***penvp)
{
	const char	*path;
	int			type;

	type = -1;
	if (argv[1] && argv[2])
	{
		print_error("cd", "too many arguments");
		return (1);
	}
	path = resolve_cd_target(argv, penvp, &type);
	return (change_dir_and_update(path, penvp, type));
}
