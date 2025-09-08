/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/08 22:00:21 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include "libft.h"
#include "executor.h"
#include "builtins.h"

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
	
	// Return the expanded path from argv
	return (argv[1]);
}

static int	change_dir_and_update(const char *path, char ***penvp)
{
	char *oldpwd;
	char *newpwd;
	char *env_old;
	char *expanded_path = NULL;
	char *home;
	int result = 0;

	if (!path)
	{
		ft_putendl_fd("cd: OLDPWD not set", STDERR_FILENO);
		return (1);
	}

	// Save old PWD to OLDPWD before changing directory
	env_old = env_get_value(*penvp, "PWD");
	if (env_old)
		oldpwd = ft_strdup(env_old);
	else
		oldpwd = getcwd(NULL, 0);

	// If path starts with $HOME (likely passed as "$HOME"/something)
	if (ft_strncmp(path, "/home/", 6) == 0)
	{
		// Just use the path as is
		expanded_path = ft_strdup(path);
	}
	else if (path[0] == '~' && (path[1] == '\0' || path[1] == '/'))
	{
		// Tilde expansion
		home = env_get_value(*penvp, "HOME");
		if (home)
		{
			if (path[1] == '\0')
				expanded_path = ft_strdup(home);
			else
			{
				expanded_path = ft_strjoin(home, path + 1);
			}
		}
		else
			expanded_path = ft_strdup(path);
	}
	else
	{
		expanded_path = ft_strdup(path);
	}

	// Change directory
	if (chdir(expanded_path) == -1)
	{
		perror("cd");
		result = 1;
	}
	else
	{
		// Get the current directory after changing
		newpwd = getcwd(NULL, 0);
		
		// Update environment variables
		if (oldpwd)
			env_set_var(penvp, "OLDPWD", oldpwd);
		if (newpwd)
			env_set_var(penvp, "PWD", newpwd);
		
		// Clean up new path
		free(newpwd);
	}
	
	// Clean up allocated memory
	free(expanded_path);
	free(oldpwd);
	
	return (result);
}

int	bi_cd(char **argv, char ***penvp)
{
	const char	*path;
	int		argc;

	// Count arguments
	argc = 0;
	while (argv[argc])
		argc++;

	// Check for too many arguments
	if (argc > 2)
	{
		ft_putendl_fd("cd: too many arguments", STDERR_FILENO);
		return (1);
	}

	path = resolve_cd_target(argv, penvp);
	return (change_dir_and_update(path, penvp));
}
