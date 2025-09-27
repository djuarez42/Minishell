/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 18:27:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 18:59:58 by ekakhmad         ###   ########.fr       */
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
# include "exec_state.h"

//Global constants
# define PROMPT "minishell$ "

/* Error helpers */
void	print_execve_error(const char *filename);
void	print_errno(const char *context);
/* Output helpers for builtins: use printf directly in code (no macros) */

#endif