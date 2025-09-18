/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:23:23 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/18 19:54:38 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include <stdbool.h>
# include <sys/types.h>
# include <sys/wait.h>

# include "lexer.h"
# include "parser.h"
# include "exec_state.h"

/* --------------------------- */
/*        Estructuras          */
/* --------------------------- */

typedef struct s_heredoc_args
{
	int				fd;
	const char		*delimiter;
	bool			quoted;
	char			**envp;
	t_exec_state	*state;
	char			*heredoc_path;
}	t_heredoc_args;

/* --------------------------- */
/*     Función principal       */
/* --------------------------- */

void			executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state);

/* --------------------------- */
/*    Gestión del entorno      */
/* --------------------------- */

void			free_split(char **split);
char			**new_envp(char **envp);
void			free_envp(char **envp_copy);
int				env_identifier_valid(const char *name);
int				env_find_index(char **envp, const char *name);
char			*env_get_value(char **envp, const char *name);
int				env_set_var(char ***penvp, const char *name, const char *value);
int				env_set_assignment(char ***penvp, const char *assignment);
int				env_unset_var(char ***penvp, const char *name);

/* --------------------------- */
/*       Redirecciones         */
/* --------------------------- */

void			handle_redirections_out(const char *filename, int *error);
void			handle_redirections_in(const char *filename, int *error);
void			handle_redirections_append(const char *filename, int *error);
int				handle_redirections(t_redir *redir, char **envp,
					t_exec_state *state);
int				handle_redirections_and_quotes(t_redir *redirs,
					char **envp, t_exec_state *state);

/* --------------------------- */
/*  Ejecución de comandos      */
/* --------------------------- */

int				execute_command(char *exec_path, t_cmd *cmd, char **envp);
int				execute_execve(char *exec_path, char **argv, char **envp);
char			*find_executable(char *cmd, char **envp);

/* --------------------------- */
/*   Expansión de variables    */
/* --------------------------- */

char			*str_append(char *dest, const char *src);
int				is_var_start(int c);
int				is_var_char(int c);
int				skip_variable_name(const char *s);
char			*expand_exit_status(t_exec_state *state);
char			*expand_env_var(const char *name, char **envp);
char			*handle_dollar_quotes_fix(const char *input, int *i,
					char **envp, t_exec_state *state);
char			*expand_variables(const char *input, char **envp,
					t_exec_state *state, t_quote_type quote);

/* --------------------------- */
/*          Heredocs           */
/* --------------------------- */

int				open_heredoc_file(t_heredoc_args *args);
void			redirect_stdin_heredoc(const char *filepath);
int				write_heredoc_lines(t_heredoc_args *args);
int				handle_redirections_heredoc(const char *delimiter, bool quoted,
					char **envp, t_heredoc_args *args);
int				handle_redirections_heredoc_with_content(char **heredoc_content,
					bool quoted, char **envp, t_exec_state *state,
					t_heredoc_args *args);
char			*build_heredoc_delimiter(const char *text);

/* --------------------------- */
/*      Utils / Errores        */
/* --------------------------- */

void			print_error_file(const char *context);
char			*interpret_ansi_c_escapes(const char *str);
char			*expand_ansi_c_string(const char *input);
int				is_dollar_string(const char *input, int pos);

#endif
