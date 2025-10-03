/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 18:27:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 16:59:54 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

//Standar libreries for minishell project
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <stdbool.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>

// src headers libraries
# include "lexer.h"
# include "parser.h"
# include "executor.h"
# include "libft.h"
# include "signals.h"
# include "builtins.h"

//Global constants
# define PROMPT "minishell$ "

/* Error helpers */
void	print_execve_error(const char *filename);
void	print_errno(const char *context);

/* Heredoc buffer for non-interactive mode */
void	heredoc_buffer_init(t_exec_state *state, const char *content);
void	heredoc_buffer_free(t_exec_state *state);
char	*heredoc_buffer_readline(t_exec_state *state);
int		heredoc_buffer_active(t_exec_state *state);

/* Heredoc helpers */
int		count_heredocs(const char *line);
char	*extract_delimiter(const char *line, int *pos);
char	**find_heredoc_delimiters(const char *cmd_line);
char	*collect_heredoc_bodies_into_buffer(char **delimiters);

/* Heredoc assignment from buffer to AST */
void	assign_heredocs_from_buffer(t_cmd *cmd_list, t_exec_state *state);

/* get_next_line */
char	*get_next_line(int fd);

/* Output helpers for builtins: use printf directly in code (no macros) */

#endif