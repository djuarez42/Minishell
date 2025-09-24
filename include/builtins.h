/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:20:50 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/24 16:55:50 by djuarez          ###   ########.fr       */
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

/*export utils*/
int	assign_or_error(char ***penvp, const char *arg);
int	validate_name_or_error(const char *arg);
int	count_env_entries(char **envp);
char	**copy_env_entries(char **envp, int n);
int	cmp_env(const void *a, const void *b);
void	print_env_entry(char *entry);
int	env_append_assignment(char ***penvp, const char *arg);

/* Individual builtins */
int	bi_echo(t_cmd *cmd);
int	bi_pwd(void);
int	bi_env(char **argv, char **envp);
int	bi_cd(char **argv, char ***penvp);
int	bi_export(char **argv, char ***penvp);
int	bi_unset(char **argv, char ***penvp);
int	bi_exit(char **argv);

#endif
