/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo_pwd_env.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/07 21:05:13 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "libft.h"
#include "builtins.h"

static int	is_n_flag(const char *s)
{
	size_t	i;

	if (!s || s[0] != '-' || s[1] != 'n')
		return (0);
	i = 2;
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0');
}

static int bi_echo_with_argv(t_cmd *cmd)
{
    int i = 1;
    int n_flag = 0;
    
    while (cmd->argv[i] && is_n_flag(cmd->argv[i]))
    {
        n_flag = 1;
        i++;
    }

    while (cmd->argv[i])
    {
        write(STDOUT_FILENO, cmd->argv[i], ft_strlen(cmd->argv[i]));
        if (cmd->argv[i + 1])
            write(STDOUT_FILENO, " ", 1);
        i++;
    }

    if (!n_flag)
        write(STDOUT_FILENO, "\n", 1);

    return 0;
}

int bi_echo(t_cmd *cmd)
{
    if (cmd->argv_final_text)
    {
	// printf("DEBUG bi_echo: usando argv_final_text: '%s'\n", cmd->argv_final_text);
        // For argv_final_text, we can't parse options like -n properly
        // So we'll just use the normal argv processing path
    }

    // Process arguments normally
    return bi_echo_with_argv(cmd);
}

int	bi_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("pwd");
		return (1);
	}
	ft_putendl_fd(cwd, STDOUT_FILENO);
	free(cwd);
	return (0);
}

int	bi_env(char **argv, char **envp)
{
	int	i;

	if (!envp)
		return (0);
	
	// Check if an argument was provided - if so, try to execute it
	if (argv && argv[1])
	{
		// Check if the command exists in PATH
		ft_putstr_fd("env: '", STDERR_FILENO);
		ft_putstr_fd(argv[1], STDERR_FILENO);
		ft_putstr_fd("': No such file or directory\n", STDERR_FILENO);
		return (127); // Command not found error code
	}
	
	// No arguments - print environment
	i = 0;
	while (envp[i])
	{
		ft_putendl_fd(envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}
