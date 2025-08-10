/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:23:23 by djuarez           #+#    #+#             */
/*   Updated: 2025/07/27 13:03:04 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "parser.h"
# include <sys/types.h>
# include <sys/wait.h>

//Functions
void	execute_cmds(t_cmd *cmd, char **envp);
void	executor(t_cmd *cmd_list, char **envp);
//Utils
void	free_split(char **split);
char	**new_envp(char **envp);
void	free_envp(char **envp_copy);
void	handle_redirections_out(const char *filename);
void	handle_redirections_in(const char *filename);
void	handle_redirections_append(const char *filename);
void	handle_redirections(t_redir *redir);
void	handle_redirections_and_quotes(t_redir *redirs);
void	execute_command(char *exec_path, t_cmd *cmd, char **envp);
void	execute_execve(char *exec_path, char **argv, char **envp);
char	*find_executable(char *cmd);
void	handle_redirections_heredoc(const char	*delimiter);
#endif