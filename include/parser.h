/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 20:17:45 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 22:04:27 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "lexer.h"

# define MAX_ARGS 100

typedef struct s_redir
{
	char				*file;
	int					type;
	bool				quoted;
	char				**heredoc_content;  // Pre-collected heredoc lines
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_quote_type	*argv_quote;
	t_redir			*redirs;
	int				pipe;
	struct s_cmd	*next;
}	t_cmd;

void	free_cmds(t_cmd *cmd);
void	free_redirs(t_redir *redir);
int		add_argument(t_cmd *cmd, char *value, t_quote_type quote, int *argc);
t_cmd	*parser_tokens(t_token *tokens);
t_token	*parse_arguments(t_token *cur, t_cmd *cmd);
t_token	*parse_cmd_block(t_token *cur, t_cmd *cmd);
t_token	*parse_redirections(t_token *cur, t_cmd *cmd);
t_redir	*create_redir(t_token *cur);
void	print_redirs(t_redir *redir);
void	print_cmd_list(t_cmd *cmd_list);
bool	is_quoted(const char *str);
int		init_cmd_args(t_cmd *cmd);
int		process_token(t_cmd *cmd, t_token *cur, int *argc);
t_cmd	*create_cmd_node(t_token **cur);
void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd);
char	**collect_heredoc_content(const char *delimiter, bool quoted);

#endif
