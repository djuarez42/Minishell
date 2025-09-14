/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:28:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 01:00:55 by djuarez          ###   ########.fr       */
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
	TOKEN_REDIRECT_IN,// <		#2
	TOKEN_REDIRECT_OUT,// >		#3
	TOKEN_HEREDOC,// <<			#4
	TOKEN_APPEND,// >>			#5
	TOKEN_EOF,// end of file 	#6
	TOKEN_IN_FILE,// Input file #7
	TOKEN_OUT_FILE// Output file#8
}	t_token_type;

//Quotes
typedef enum e_quote_type
{
	QUOTE_NONE,   //#0
	QUOTE_SINGLE, //#1
	QUOTE_DOUBLE, //#2
	QUOTE_MIXED,  //#3
	QUOTE_DOLLAR  //#4
}	t_quote_type;

//Fragment structure
typedef struct s_fragment 
{
	char				*text;
	char				*expanded_text;
	t_quote_type		quote_type;
	bool				has_space_after;
	bool				is_variable;
	struct s_fragment	*next;
} t_fragment;

//Token structure
//Function prototype moved below the fragment structure
char *concat_fragments(t_fragment *fragments);
typedef struct s_token 
{
	t_token_type	type;
	t_fragment		*fragments;
	bool			has_space_before;
	char			*final_text;
	struct s_token	*next;
} t_token;

// --- MISSING FUNCTION PROTOTYPES FOR TOKENIZER ---
t_fragment *parse_fragments(const char *input);
t_token *create_token(t_token_type type, bool has_space_before);
t_fragment *new_fragment(const char *text, size_t len, t_quote_type quote_type, bool has_space_after);
void append_fragment(t_fragment **list, t_fragment *new_frag);
void append_token(t_token **list, t_token *new_token);
t_token *build_token_list_from_fragments(t_token *raw_tokens);
void free_fragments(t_fragment *frag);
void free_token_list(t_token *tokens);
void free_tokens(t_token *tokens);

enum lexer_state 
{
	LS_NORMAL,
	LS_IN_SINGLE,
	LS_IN_DOUBLE,
	LS_ESCAPE
};

//(Tokenize/lexer)utils
t_token			*tokenize_input(const char *input);
int				is_quote(char c);
int				is_operator(char c);
t_token_type 	determine_token_type(char *str, t_quote_type quote);
t_token			*append_token_eof(t_token *head);
char			*remove_quotes(char *str);
bool			are_quotes_closed(const char *input);
char			*extract_quoted_segment(const char *input, int *len);
int				skip_spaces(const char *s, int i);
char			*handle_quoted_part(const char *input, int *i, char *tmp,
					t_quote_type *last_quote);
char			*handle_plain_text(const char *input, int *i, char *tmp);
int 			check_unmatched_quotes(const char *input);
t_fragment 		*parse_mixed_fragments(const char *text);
void            assign_token_types(t_token *head);
t_fragment		*handle_backslashes(const char *text, int *i);


// Debug functions
void			print_fragments(t_fragment *fragments);
void 			print_tokens_raw(t_token *tokens);


#endif

