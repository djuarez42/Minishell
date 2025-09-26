/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:23:23 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 18:15:18 by ekakhmad         ###   ########.fr       */
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
# include <sys/stat.h>

/* --------------------------- */
/*        Structures           */
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

typedef struct s_fork_ctx
{
	t_cmd			*cur;
	char			**envp;
	t_exec_state	*state;
	int				(*pipes)[2];
	size_t			i;
	size_t			n_cmds;
	size_t			n_pipes;
	pid_t			*pids;
}	t_fork_ctx;

typedef struct s_wait_ctx
{
	int		status;
	pid_t	w;
	pid_t	last_pid;
	int		final_status;
	size_t	left;
}	t_wait_ctx;

typedef struct s_dollar_ctx
{
	char			**envp;
	t_exec_state	*state;
	t_quote_type	quote;
}	t_dollar_ctx;

/* --------------------------- */
/*     Main function           */
/* --------------------------- */

void			executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state);

/* --------------------------- */
/*    ENV managment            */
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
/*       Redirections          */
/* --------------------------- */

void			handle_redirections_out(const char *filename, int *error);
void			handle_redirections_in(const char *filename, int *error);
void			handle_redirections_append(const char *filename, int *error, \
					t_exec_state *state);
int				handle_redirections(t_redir *redir, char **envp,
					t_exec_state *state);
int				handle_redirections_and_quotes(t_redir *redirs,
					char **envp, t_exec_state *state);

/* --------------------------- */
/*  Command execution          */
/* --------------------------- */

int				execute_command(char *exec_path, t_cmd *cmd, char **envp);
int				execute_execve(char *exec_path, char **argv, char **envp);
char			*find_executable(char *cmd, char **envp);
char			*execute_command_substitution(const char *cmd, char **envp);

/* --------------------------- */
/*   Variables expansion       */
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
					t_heredoc_args *args);

char			*build_heredoc_delimiter(const char *text);

/* --------------------------- */
/*      Utils / Errors         */
/* --------------------------- */

void			print_error_file(const char *context);
char			*interpret_ansi_c_escapes(const char *str);
char			*expand_ansi_c_string(const char *input);
int				is_dollar_string(const char *input, int pos);
char			*handle_dollar_string(const char *input, int *i);
//para ordenar 
int				init_pipes_and_n(int n_cmds, int (**pipes)[2], size_t *n_pipes);
int				fork_and_run_pipeline(t_cmd *start, size_t n_cmds,
					char **envp, t_exec_state *state);
size_t			count_pipeline_cmds(t_cmd *start);
void			close_all_pipes(int (*pipes)[2], size_t n_pipes);
int				setup_pipes(t_fork_ctx *ctx, size_t n_cmds);
int				fork_and_init_children(t_cmd *start, size_t n_cmds,
					t_fork_ctx *ctx, pid_t *pids);
int				wait_and_cleanup(pid_t *pids, size_t n_cmds);
void			wire_child_pipes(size_t idx, size_t n_cmds, int (*pipes)[2]);
int				wait_pipeline(pid_t *pids, size_t n);
int				create_pipes(int (**pipes)[2], size_t n_pipes);

/* fd guard helpers for parent-side redirs */
int				fd_guard_begin(int saved[3]);
void			fd_guard_end(int saved[3]);

#endif
