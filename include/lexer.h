/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:28:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 11:03:33 by ekakhmad         ###   ########.fr       */
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
	TOKEN_IN_FILE,// Input file #7
	TOKEN_OUT_FILE// Output file#8
}	t_token_type;

//Quotes
typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
	QUOTE_MIXED
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
int				operator_len(const char *s);
void			free_tokens(char **tokens, int count);
int				is_blank(const char *s);
t_token_type	determine_token_type(char *str);
t_token			*build_token_list(char **tokens, t_quote_type *quotes);
void			print_token_list(t_token *token);
void			free_token_list(t_token *head);
t_token			*append_token_eof(t_token *head);
const char		*token_type_str(t_token_type type);
char			*remove_quotes(char *str);
bool			are_quotes_closed(const char *input);
char			*extract_quoted_segment(const char *input, int *len);
int				skip_spaces(const char *s, int i);
char			*handle_quoted_part(const char *input, int *i, char *tmp,
					t_quote_type *last_quote);
char			*handle_plain_text(const char *input, int *i, char *tmp);
void			add_token(char **tokens, int *tok_i, char **tmp);
char			**clean_input_quotes(const char *input,
					t_quote_type **quotes_out);
char			**reconstruct_words(const char *input, t_quote_type *last_quote,
					t_quote_type **quotes_out);
int				should_add_token(const char *input, int i);
void			check_and_add_token(char **tokens, int *tok_i, char **tmp);
int				process_spaces_and_quotes(const char *input, int i, char **tmp,
					t_quote_type *last_quote);
char			*str_append(char *base, const char *add);
int				init_tokens_and_quotes(char ***tokens_out,
					t_quote_type **quotes_out);
const char		*quote_type_str(t_quote_type q);
char			*strip_comments(const char *input);
void            classify_redirection_files(t_token *token_list);

// Additional function prototypes for lexer_utils5.c
t_fragment      *duplicate_fragment(t_fragment *frag);
t_token         *create_token_from_fragments(t_fragment *frag, bool space_before);
void            assign_token_types(t_token *head);

#endif

