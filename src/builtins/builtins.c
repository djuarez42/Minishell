/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:21:10 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/09 14:21:10 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "libft.h"
#include "minishell.h"
#include "executor.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int is_n_flag(const char *s)
{
    size_t i;

    if (!s || s[0] != '-' || s[1] != 'n')
        return (0);
    i = 2;
    while (s[i] == 'n')
        i++;
    return (s[i] == '\0');
}

int is_builtin(const char *cmd)
{
    if (!cmd)
        return (0);
    return (!ft_strncmp(cmd, "echo", 5) ||
            !ft_strncmp(cmd, "cd", 3) ||
            !ft_strncmp(cmd, "pwd", 4) ||
            !ft_strncmp(cmd, "export", 7) ||
            !ft_strncmp(cmd, "unset", 6) ||
            !ft_strncmp(cmd, "env", 4) ||
            !ft_strncmp(cmd, "exit", 5));
}

int bi_echo(char **argv)
{
    int i;
    int n_flag;

    i = 1;
    n_flag = 0;
    while (argv[i] && is_n_flag(argv[i]))
    {
        n_flag = 1;
        i++;
    }
    while (argv[i])
    {
        write(STDOUT_FILENO, argv[i], ft_strlen(argv[i]));
        if (argv[i + 1])
            write(STDOUT_FILENO, " ", 1);
        i++;
    }
    if (!n_flag)
        write(STDOUT_FILENO, "\n", 1);
    return (0);
}

int bi_pwd(void)
{
    char *cwd;

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

int bi_env(char **envp)
{
    int i;

    if (!envp)
        return (0);
    i = 0;
    while (envp[i])
    {
        ft_putendl_fd(envp[i], STDOUT_FILENO);
        i++;
    }
    return (0);
}

static void update_pwd_vars(char ***penvp)
{
    char    *oldpwd;
    char    *newpwd;
    char    *env_old;

    // Determine oldpwd: prefer PWD from env, fallback to getcwd
    env_old = env_get_value(*penvp, "PWD");
    if (env_old)
        oldpwd = ft_strdup(env_old);
    else
        oldpwd = getcwd(NULL, 0);

    // Determine newpwd from current directory after chdir
    newpwd = getcwd(NULL, 0);

    if (oldpwd)
        env_set_var(penvp, "OLDPWD", oldpwd);
    if (newpwd)
        env_set_var(penvp, "PWD", newpwd);

    free(oldpwd);
    free(newpwd);
}

int bi_cd(char **argv, char ***penvp)
{
    const char *path;

    if (!argv[1])
    {
        ft_putendl_fd("minishell: cd: missing operand", STDERR_FILENO);
        return (1);
    }
    path = argv[1];

    // Attempt to change directory
    if (chdir(path) == -1)
    {
        perror("cd");
        return (1);
    }

    // Update PWD and OLDPWD in environment
    update_pwd_vars(penvp);
    return (0);
}

int bi_export(char **argv, char ***penvp)
{
    int i;
    int status;

    if (!argv[1])
        return bi_env(*penvp);
    status = 0;
    i = 1;
    while (argv[i])
    {
        if (ft_strchr(argv[i], '='))
        {
            if (!env_set_assignment(penvp, argv[i]))
            {
                ft_putstr_fd("minishell: export: `", STDERR_FILENO);
                ft_putstr_fd(argv[i], STDERR_FILENO);
                ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
                status = 1;
            }
        }
        else
        {
            if (!env_identifier_valid(argv[i]) || !env_set_var(penvp, argv[i], ""))
            {
                ft_putstr_fd("minishell: export: `", STDERR_FILENO);
                ft_putstr_fd(argv[i], STDERR_FILENO);
                ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
                status = 1;
            }
        }
        i++;
    }
    return status;
}

int bi_unset(char **argv, char ***penvp)
{
    int i;
    int status;

    status = 0;
    i = 1;
    while (argv[i])
    {
        if (!env_identifier_valid(argv[i]) || !env_unset_var(penvp, argv[i]))
        {
            ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
            ft_putstr_fd(argv[i], STDERR_FILENO);
            ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
            status = 1;
        }
        i++;
    }
    return status;
}

int bi_exit(char **argv)
{
    int status;

    status = 0;
    if (argv[1])
        status = ft_atoi(argv[1]);
    // In child, caller will exit(status). In parent, run_builtin_in_parent will exit.
    return (status & 0xFF);
}

static int dispatch_builtin(char **argv, char ***penvp)
{
    if (!ft_strncmp(argv[0], "echo", 5))
        return bi_echo(argv);
    if (!ft_strncmp(argv[0], "pwd", 4))
        return bi_pwd();
    if (!ft_strncmp(argv[0], "env", 4))
        return bi_env(*penvp);
    if (!ft_strncmp(argv[0], "cd", 3))
        return bi_cd(argv, penvp);
    if (!ft_strncmp(argv[0], "export", 7))
        return bi_export(argv, penvp);
    if (!ft_strncmp(argv[0], "unset", 6))
        return bi_unset(argv, penvp);
    if (!ft_strncmp(argv[0], "exit", 5))
        return bi_exit(argv);
    return (1);
}

int run_builtin_in_child(t_cmd *cmd, char ***penvp)
{
    return dispatch_builtin(cmd->argv, penvp);
}

int run_builtin_in_parent(t_cmd *cmd, char ***penvp)
{
    int status;

    status = dispatch_builtin(cmd->argv, penvp);
    if (!ft_strncmp(cmd->argv[0], "exit", 5))
        exit(status);
    return status;
}
