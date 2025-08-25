/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:23:23 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/25 14:10:46 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "parser.h"
# include <sys/types.h>
# include <sys/wait.h>

//Structs

typedef struct s_exec_state
{
	int	last_status;
}	t_exec_state;

typedef struct s_heredoc_args
{
	int				fd;
	const char		*delimiter;
	bool			quoted;
	char			**envp;
	t_exec_state	*state;
}	t_heredoc_args;

//Functions
void	execute_cmds(t_cmd *cmd, char **envp);
void	executor(t_cmd *cmd_list, char ***penvp, t_exec_state *state);

// Utils
void	free_split(char **split);
char	**new_envp(char **envp);
void	free_envp(char **envp_copy);

// Env management
int		env_identifier_valid(const char *name);
int		env_find_index(char **envp, const char *name);
char	*env_get_value(char **envp, const char *name);
int		env_set_var(char ***penvp, const char *name, const char *value);
int		env_set_assignment(char ***penvp, const char *assignment);
int		env_unset_var(char ***penvp, const char *name);

// Redirections
void	handle_redirections_out(const char *filename, int *error);
void	handle_redirections_in(const char *filename, int *error);
void	handle_redirections_append(const char *filename, int *error);
int		handle_redirections(t_redir *redir, char **envp);
int		handle_redirections_and_quotes(t_redir *redirs, char **envp);

// Exec helpers
void	execute_command(char *exec_path, t_cmd *cmd, char **envp);
void	execute_execve(char *exec_path, char **argv, char **envp);
char	*find_executable(char *cmd, char **envp);

// expand variables
char	*str_append(char *dest, const char *src);
int		is_var_start(int c);
int		is_var_char(int c);
int		skip_variable_name(const char *s);
char	*expand_exit_status(t_exec_state *state);
char	*expand_env_var(const char *name, char **envp);
char	*handle_special_dollar(const char *input, int *i, t_exec_state *state);
char	*extract_plain_text(const char *input, int *i, char *tmp);
char	*handle_dollar(const char *input, int *i, char **envp,
			t_exec_state *state);
int		expand_argv(char **argv, t_quote_type *argv_quote, char **envp,
			t_exec_state *state);
int		expand_redirs(t_redir *redir, char **envp, t_exec_state *state);
int		expand_cmd_inplace(t_cmd *cur, char **envp, t_exec_state *state);
char	*expand_variables(const char *input, char **envp, t_exec_state *state);


// Heredoc utils
int		open_heredoc_file(void);
void	redirect_stdin_heredoc(void);
int		write_heredoc_lines(t_heredoc_args *args);
int		handle_redirections_heredoc(const char *delimiter, bool quoted,
			char **envp, t_heredoc_args *args);
#endif
