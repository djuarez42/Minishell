/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 20:17:45 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 17:04:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <stdbool.h>
# include "lexer.h"       // define t_quote_type, t_token, etc.
# include "exec_state.h"  // define t_exec_state

# define MAX_ARGS 32645

typedef struct s_redir
{
    char                *file;
    int                 type;
    bool                quoted;
    char                **heredoc_content;
    struct s_redir      *next;
    t_fragment          *fragments;
}   t_redir;

typedef struct s_cmd
{
    char            **argv;
    t_quote_type    *argv_quote;
    char            **argv_final_text;
    bool            argv_space[MAX_ARGS];
    bool            *argv_first_word;
    t_redir         *redirs;
    int             pipe;
    struct s_cmd    *next;
}   t_cmd;

typedef struct s_proc_ctx
{
    t_cmd           *cmd;
    char            **envp;
    t_exec_state    *state;
    int             *argc_argv;         // para argv
    int             *argc_final_text;   // para argv_final_text
} t_proc_ctx;

/* Prototipos del parser (ya con envp/state) */
void    free_cmds(t_cmd *cmd);
void    free_redirs(t_redir *redir);

t_cmd   *parser_tokens(t_token *tokens, char **envp, t_exec_state *state);
t_token *parse_arguments(t_token *cur, t_cmd *cmd,
            char **envp, t_exec_state *state);
t_token *parse_cmd_block(t_token *cur, t_cmd *cmd, char **envp, t_exec_state *state);
t_token *parse_redirections(t_token *cur, t_cmd *cmd);
t_redir *create_redir(t_token *cur);

void    print_redirs(t_redir *redir);
void    print_cmd_list(t_cmd *cmd_list);

char    **process_token(t_token *tok, char **argv, int *argc, char **envp);
char    **process_token_with_quotes(t_token *tok, t_proc_ctx *ctx);
void    free_str_array(char **arr);

t_cmd   *create_cmd_node(t_token **cur, char **envp, t_exec_state *state);
void    add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd);
char    **collect_heredoc_content(const char *delimiter, bool quoted);

/* Aux de expansión en parser */
void    expand_fragments(t_token *tok, char **envp, t_exec_state *state);
char    *expand_fragment(const char *text, t_quote_type quote, char **envp, t_exec_state *state);
char	*expand_tilde_bash(const char *text, char **envp);
//char    *build_final_text(t_token *tok, char **envp, t_exec_state *state);
int      should_expand_fragment(t_fragment *frag);
char    *concat_token_fragments(t_token *tok, char **envp, t_exec_state *state);
char    *concat_final_text(t_token *tok);
char    **build_argv_from_fragments(t_token *tok, t_proc_ctx *ctx);
char    **build_words_from_token(t_token *tok, int *out_count);
char    **ft_split_spaces(const char *s);
t_quote_type detect_combined_quote(t_fragment *frags);
t_token *expand_token_to_tokens(t_token *tok, char **envp, t_exec_state *state);

#endif

