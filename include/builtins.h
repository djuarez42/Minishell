/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:20:50 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/15 19:05:21 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "parser.h"

/* Detection */
int	is_builtin(const char *cmd);

/* Execute in parent (no pipeline) */
int	run_builtin_in_parent(t_cmd *cmd, char ***penvp);

/* Execute in child (pipeline member) */
int	run_builtin_in_child(t_cmd *cmd, char ***penvp);

/* Individual builtins */
int bi_echo(t_cmd *cmd);
int	bi_pwd(void);
int	bi_env(char **argv, char **envp);
int	bi_cd(char **argv, char ***penvp);
int	bi_export(char **argv, char ***penvp);
int	bi_unset(char **argv, char ***penvp);
int	bi_exit(char **argv);

#endif
