/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/30 21:47:36 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include "libft.h"
#include "executor.h"
#include "builtins.h"

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

static const char	*resolve_cd_target(char **argv, char ***penvp)
{
	const char	*path;

	if (!argv[1])
	{
		path = env_get_value(*penvp, "HOME");
		return (path);
	}
	
	// Handle cd - case (change to OLDPWD)
	if (ft_strncmp(argv[1], "-", 2) == 0)
	{
		path = env_get_value(*penvp, "OLDPWD");
		if (path)
		{
			char *temp = ft_strdup(path);
			if (temp)
			{
				ft_putendl_fd(temp, STDOUT_FILENO);  // Print the directory we're changing to
				free(temp);
			}
		}
		return (path);
	}
	
	return (argv[1]);
}

static int	change_dir_and_update(const char *path, char ***penvp)
{
	if (!path)
	{
		ft_putendl_fd("cd: OLDPWD not set", STDERR_FILENO);
		return (1);
	}
	
	if (chdir(path) == -1)
	{
		perror("cd");
		return (1);
	}
	update_pwd_vars(penvp);
	return (0);
}

int	bi_cd(char **argv, char ***penvp)
{
	const char	*path;

	path = resolve_cd_target(argv, penvp);
	return (change_dir_and_update(path, penvp));
}
