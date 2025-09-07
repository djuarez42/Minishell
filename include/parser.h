/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 20:17:45 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/07 00:56:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <stdbool.h>
# include "lexer.h"       // define t_quote_type, t_token, etc.
# include "exec_state.h"  // define t_exec_state

# define MAX_ARGS 100

typedef struct s_redir
{
    char                *file;
    int                 type;
    bool                quoted;
    char                **heredoc_content;
    struct s_redir      *next;
}   t_redir;

typedef struct s_cmd
{
    char            **argv;
    t_quote_type    *argv_quote;
    t_redir         *redirs;
    int             pipe;
    struct s_cmd    *next;
}   t_cmd;

typedef struct s_proc_ctx
{
    t_cmd        *cmd;
    int          *argc;
    char        **envp;
    t_exec_state *state;
}   t_proc_ctx;

/* Prototipos del parser (ya con envp/state) */
void    free_cmds(t_cmd *cmd);
void    free_redirs(t_redir *redir);
int     add_argument(t_cmd *cmd, char *value, t_quote_type quote, int *argc);

t_cmd   *parser_tokens(t_token *tokens, char **envp, t_exec_state *state);
t_token *parse_arguments(t_token *cur, t_cmd *cmd,
            char **envp, t_exec_state *state);
t_token *parse_cmd_block(t_token *cur, t_cmd *cmd, char **envp, t_exec_state *state);
t_token *parse_redirections(t_token *cur, t_cmd *cmd);
t_redir *create_redir(t_token *cur);

void    print_redirs(t_redir *redir);
void    print_cmd_list(t_cmd *cmd_list);
bool    is_quoted(const char *str);
int     init_cmd_args(t_cmd *cmd);

char    **process_token(t_token *tok, char **argv, int *argc, char **envp);
char    **process_token_with_quotes(t_token *tok, t_proc_ctx *ctx);

t_cmd   *create_cmd_node(t_token **cur, char **envp, t_exec_state *state);
void    add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd);
char    **collect_heredoc_content(const char *delimiter, bool quoted);

/* Aux de expansión en parser */
char    *expand_fragment(const char *text, t_quote_type quote, char **envp, t_exec_state *state);
char    *build_final_text(t_token *tok, char **envp, t_exec_state *state);
int      should_expand_fragment(t_fragment *frag);
char    *concat_token_fragments(t_token *tok, char **envp, t_exec_state *state);


#endif
