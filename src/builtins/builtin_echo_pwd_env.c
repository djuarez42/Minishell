/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo_pwd_env.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/07 21:00:57 by djuarez          ###   ########.fr       */
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

int bi_echo(t_cmd *cmd)
{
    int i = 1;
    int n_flag = 0;

    if (cmd->argv_final_text && cmd->argv_final_text[0])
    {
        printf("DEBUG bi_echo: usando argv_final_text\n");

        // manejar flags -n
        while (cmd->argv_final_text[i] && is_n_flag(cmd->argv_final_text[i]))
        {
            n_flag = 1;
            i++;
        }

        // imprimir argumentos
        while (cmd->argv_final_text[i])
        {
            write(STDOUT_FILENO, cmd->argv_final_text[i], ft_strlen(cmd->argv_final_text[i]));
            if (cmd->argv_final_text[i + 1])
                write(STDOUT_FILENO, " ", 1);
            i++;
        }

        if (!n_flag)
            write(STDOUT_FILENO, "\n", 1);

        return 0;
    }

    printf("DEBUG bi_echo: argv_final_text vacío o NULL\n");
    return 0;
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
