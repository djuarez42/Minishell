/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_manager.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 18:34:15 by ekakhmad          #+#    #+#             */
/*   Updated: 2023/08/31 19:12:48 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FD_MANAGER_H
# define FD_MANAGER_H

# include <stdbool.h>
# include "minishell.h"
# include "executor.h"
# include "lexer.h"

/* Redirection type constants mapping to token types */
# define REDIR_IN       2  /* < */
# define REDIR_OUT      3  /* > */
# define REDIR_HEREDOC  4  /* << */
# define REDIR_APPEND   5  /* >> */

typedef struct s_fd_state
{
	int		original_stdin;
	int		original_stdout;
	int		original_stderr;
	bool	stdin_modified;
	bool	stdout_modified;
	bool	stderr_modified;
}	t_fd_state;

t_fd_state	*fd_state_init(void);
void		fd_state_save_original(t_fd_state *state);
void		fd_state_restore_all(t_fd_state *state);
void		fd_state_cleanup(t_fd_state *state);
int			setup_input_redirection(const char *filename, t_fd_state *state);
int			setup_output_redirection(const char *filename, bool append, t_fd_state *state);
int			setup_heredoc_redirection(int fd, t_fd_state *state);
int			setup_redirections(t_redir *redirs, char **envp, t_exec_state *state);
int			setup_redirections_parent(t_redir *redirs, char **envp,
				t_exec_state *state, t_fd_state **fd_state);
void		cleanup_redirections_parent(t_fd_state *fd_state);

#endif
