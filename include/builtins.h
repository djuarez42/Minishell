/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:20:50 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/30 17:25:19 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "parser.h"

/* ************************************************************************** */
/*                              Builtins Detection                            */
/* ************************************************************************** */

int					is_builtin(const char *cmd);

/* ************************************************************************** */
/*                       Execution in Parent / Child                          */
/* ************************************************************************** */

int					run_bi_in_parent(t_cmd *cmd, char ***penvp);
int					run_builtin_in_child(t_cmd *cmd, char ***penvp);

/* ************************************************************************** */
/*                              Export Utilities                              */
/* ************************************************************************** */

int					assign_or_error(char ***penvp, const char *arg);
int					validate_name_or_error(const char *arg);
int					count_env_entries(char **envp);
char				**copy_env_entries(char **envp, int n);
int					cmp_env(const void *a, const void *b);
void				print_env_entry(char *entry);
int					env_append_assignment(char ***penvp, const char *arg);

int					needs_quotes(const char *val);
size_t				calc_extra_quotes(const char *val);
char				*build_quoted_value(const char *val, size_t extra);
char				*get_name_from_plus_equal(const char *arg);
void				print_export_ident_error(const char *s);
char				*dup_name(const char *env);

/* ************************************************************************** */
/*                            Individual Builtins                             */
/* ************************************************************************** */

int					bi_echo(t_cmd *cmd);
int					bi_pwd(void);
int					bi_env(char **argv, char **envp);
int					bi_cd(char **argv, char ***penvp);
int					bi_export(char **argv, char ***penvp);
int					bi_unset(char **argv, char ***penvp);
int					bi_exit(char **argv);

/* ************************************************************************** */
/*                          Export Helpers                                    */
/* ************************************************************************** */

void				print_exported_env(char **envp);

/* helpers moved to separate compilation unit */
int					unset_print_invalid_option(const char *arg);
int					exit_is_valid_number(const char *s);
int					exit_print_num_args(const char *arg);
int					exit_print_many_args(void);

/* export utils (shared within builtins) */
int					export_utils_count_env(char **envp);
char				*export_utils_dup_name(const char *env);
int					export_utils_cmp_env(const void *a, const void *b);
char				**export_utils_copy_env(char **envp, int *out_n);
char				*export_utils_quote_value(const char *val);
void				export_utils_print_no_value(const char *name);
int					export_utils_should_skip_underscore(const char *name);
void				export_utils_print_with_value(const char *name,
						const char *val);

#endif
