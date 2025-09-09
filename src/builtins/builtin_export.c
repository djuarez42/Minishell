/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/09 21:50:03 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "libft.h"
#include "executor.h"
#include "builtins.h"

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

static void print_sorted_env(char **envp)
{
    int i, j;
    int count = 0;
    char *temp;
    char **env_copy;

    // Count environment variables
    while (envp[count])
        count++;

    // Create a copy of the environment
    env_copy = (char **)malloc(sizeof(char *) * (count + 1));
    if (!env_copy)
        return;
    
    // Copy environment variables
    for (i = 0; i < count; i++)
        env_copy[i] = ft_strdup(envp[i]);
    env_copy[count] = NULL;
    
    // Sort environment variables (bubble sort)
    for (i = 0; i < count - 1; i++)
    {
        for (j = 0; j < count - i - 1; j++)
        {
            if (ft_strcmp(env_copy[j], env_copy[j + 1]) > 0)
            {
                temp = env_copy[j];
                env_copy[j] = env_copy[j + 1];
                env_copy[j + 1] = temp;
            }
        }
    }
    
    int is_tty = isatty(STDOUT_FILENO);
    for (i = 0; i < count; i++)
    {
        char *equals = ft_strchr(env_copy[i], '=');
        if (equals)
        {
            if (is_tty)
            {
                // Print in format: declare -x NAME="VALUE"
                ft_putstr_fd("declare -x ", STDOUT_FILENO);
                write(STDOUT_FILENO, env_copy[i], equals - env_copy[i]);
                ft_putstr_fd("=\"", STDOUT_FILENO);
                ft_putstr_fd(equals + 1, STDOUT_FILENO);
                ft_putendl_fd("\"", STDOUT_FILENO);
            }
            else
            {
                // Print in format: NAME=VALUE
                ft_putendl_fd(env_copy[i], STDOUT_FILENO);
            }
        }
        else
        {
            if (is_tty)
            {
                // Print in format: declare -x NAME
                ft_putstr_fd("declare -x ", STDOUT_FILENO);
                ft_putendl_fd(env_copy[i], STDOUT_FILENO);
            }
            else
            {
                // Print just NAME
                ft_putendl_fd(env_copy[i], STDOUT_FILENO);
            }
        }
    }
    
    // Free allocated memory
    for (i = 0; i < count; i++)
        free(env_copy[i]);
    free(env_copy);
}

int	bi_export(char **argv, char ***penvp)
{
	int	i;
	int	status;

	if (!argv[1])
	{
		print_sorted_env(*penvp);
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
