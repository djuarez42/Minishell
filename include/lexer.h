/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:28:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 00:08:30 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdlib.h>
# include <stdio.h>
# include "libft.h"
# include <stdbool.h>

//Define token types
typedef enum e_token_type
{
	TOKEN_WORD,// normal word   #0
	TOKEN_PIPE,// pipe '|'      #1
	TOKEN_REDIRECT_IN,// 		#2
	TOKEN_REDIRECT_OUT,// >		#3
	TOKEN_HEREDOC,// <<			#4
	TOKEN_APPEND,// >>			#5
	TOKEN_EOF,// end of file 	#6
}	t_token_type;

//Quotes
typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_type;

//Fragment structure
typedef struct s_fragment 
{
	char				*text;
	t_quote_type		quote_type;
	bool				has_space_after;
	struct s_fragment	*next;
} t_fragment;

//Token structure
typedef struct s_token 
{
	t_token_type	type;
	t_fragment		*fragments;
	bool			has_space_before;
	char			*final_text;
	struct s_token	*next;
} t_token;

enum lexer_state 
{
	LS_NORMAL,
	LS_IN_SINGLE,
	LS_IN_DOUBLE,
	LS_ESCAPE
};

//Function prototypes

t_token		*tokenize_input(const char *input);
char		*extract_token_text(const char *input, int *i, int type);
int			detect_token_type(const char *input, int i);
t_token		*new_token(t_token_type type);
t_token 	*create_token(t_token_type type, bool space_before);
t_token 	*create_token_from_fragments(t_fragment *first_frag, bool space_before);
t_token		*append_token_eof(t_token *head);
void        append_token(t_token **tokens, t_token *new);
char 		*concat_fragments_for_token(t_fragment *frag);
void 		print_final_token_list(t_token *tokens);


//fragments
t_fragment *parse_fragments(const char *text);
void 		append_fragment(t_fragment **head, t_fragment *new);
t_fragment *new_fragment(const char *start, size_t len, t_quote_type qtype, bool space_after);
t_token 	*create_token_group(t_fragment *frag_head, t_token_type type);
t_token_type detect_operator_token(t_fragment *frag);
t_token 	*build_token_list_from_fragments(t_token *raw_tokens);
char 		*concat_fragments(t_fragment *frag);
t_fragment 	*duplicate_fragment(t_fragment *frag);

// Clasificación
bool       		lx_is_space(char c);
bool 			lx_is_space_between(t_fragment *a, t_fragment *b);
bool			lx_is_meta(char c);
t_token_type	lx_meta_type(const char *s, int *consumed);
int				check_unmatched_quotes(const char *input);
void			assign_token_types(t_token *tokens);

// Construcción incremental
t_token     *lx_token_open(t_token_type type);
t_fragment  *lx_frag_open(t_quote_type qt);
int         lx_frag_push_char(t_fragment *frag, char c); // 0=error
void        lx_token_add_frag(t_token *tok, t_fragment *frag);
void        lx_list_push(t_token **head, t_token *tok);

// Helpers de flujo
int			skip_spaces(const char *input, int i);
int         lx_flush_token(t_token **out, t_token **cur_tok, t_fragment **cur_frag);
int         lx_start_word_if_needed(t_token **cur_tok, t_fragment **cur_frag, t_quote_type qt);
void		print_tokens(t_token *tokens);
void 		print_token_list_from_fragments(t_token *tokens);


// Limpieza
void		free_tokens(t_token *tok);
void 		free_fragments(t_fragment *frag);

#endif

