/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:20:50 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/23 22:25:30 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "parser.h"

/* Detection */
int		is_builtin(const char *cmd);

/* Execute in parent (no pipeline) */
int		run_builtin_in_parent(t_cmd *cmd, char ***penvp);

/* Execute in child (pipeline member) */
int		run_builtin_in_child(t_cmd *cmd, char ***penvp);

/* Individual builtins */
int		bi_echo(t_cmd *cmd);
int		bi_pwd(void);
int		bi_env(char **argv, char **envp);
int		bi_cd(char **argv, char ***penvp);
int		bi_export(char **argv, char ***penvp);
int		bi_unset(char **argv, char ***penvp);
int		bi_exit(char **argv);

/* helpers used across export implementation */
void	print_exported_env(char **envp);

/* export utils (shared within builtins) */
int		export_utils_count_env(char **envp);
char	*export_utils_dup_name(const char *env);
int		export_utils_cmp_env(const void *a, const void *b);
char	**export_utils_copy_env(char **envp, int *out_n);
char	*export_utils_quote_value(const char *val);
void	export_utils_print_no_value(const char *name);
int		export_utils_should_skip_underscore(const char *name);
void	export_utils_print_with_value(const char *name, const char *val);

#endif
