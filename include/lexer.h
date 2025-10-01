/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:28:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/29 18:40:00 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <string.h>
# include "libft.h"

/* ************************************************************************** */
/*                                Token types                                 */
/* ************************************************************************** */

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_HEREDOC,
	TOKEN_APPEND,
	TOKEN_EOF,
	TOKEN_IN_FILE,
	TOKEN_OUT_FILE,
	TOKEN_NONE
}	t_token_type;

/* ************************************************************************** */
/*                                Quote types                                 */
/* ************************************************************************** */

typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
	QUOTE_MIXED,
	QUOTE_DOLLAR
}	t_quote_type;

/* ************************************************************************** */
/*                                Structures                                  */
/* ************************************************************************** */

typedef struct s_fragment
{
	char				*text;
	char				*expanded_text;
	t_quote_type		quote_type;
	bool				has_space_after;
	bool				is_variable;
	struct s_fragment	*next;
}	t_fragment;

typedef struct s_token
{
	t_token_type	type;
	t_fragment		*fragments;
	bool			has_space_before;
	char			*final_text;
	struct s_token	*next;
}	t_token;

/* ************************************************************************** */
/*                        Structure management                                */
/* ************************************************************************** */

t_token			*create_token(t_token_type type, bool has_space_before);
t_fragment		*new_fragment(const char *text, size_t len,
					t_quote_type quote_type, bool has_space_after);
char			*concat_fragments(t_fragment *fragments);
t_token			*append_token_eof(t_token *head);
void			append_fragment(t_fragment **list, t_fragment *new_frag);
void			free_fragments(t_fragment *frag);
void			free_token_list(t_token *tokens);

/* ************************************************************************** */
/*                           Fragment helpers                                 */
/* ************************************************************************** */

size_t			calc_total_length(t_fragment *frag);
void			copy_fragments_to_buffer(t_fragment *frag, char *res);
void			append_char_to_buf(char **buf, char c);
void			handle_backslash_in_double(const char *text, int *i,
					char **buf);
char			*reduce_backslashes_literal(int count, char next_char);
char			*collect_double_quote_text(const char *text, int *i);
bool			compute_space_after(const char *text, int i);

/* ************************************************************************** */
/*                           Tokenization core                                */
/* ************************************************************************** */

t_token_type	determine_token_type(char *str, t_quote_type quote);
t_token			*tokenize_input(const char *input);

/* ************************************************************************** */
/*                           Token builders                                   */
/* ************************************************************************** */

t_token			*create_operator_token(t_fragment **cur, bool *space_before);
t_token			*create_word_token(t_fragment **cur, bool *space_before);
void			skip_empty_fragments(t_fragment **cur);
t_token			*build_tokens_from_fragments(t_fragment *cur);

/* ************************************************************************** */
/*                            Quote helpers                                   */
/* ************************************************************************** */

char			*remove_quotes(char *str);
int				check_unmatched_quotes(const char *input);
t_fragment		*parse_mixed_fragments(const char *text);
void			skip_until_closing_quote(const char *text, int *i, char quote);

/* ************************************************************************** */
/*                            Lexer helpers                                   */
/* ************************************************************************** */

t_fragment		*handle_backslashes(const char *text, int *i);
t_fragment		*handle_backslashes_even_dollar(int keep,
					const char *text, int *i);
t_fragment		*handle_backslashes_odd_dollar(int keep,
					const char *text, int *i);
t_fragment		*handle_backslashes_literal(int count, const char *text,
					int *i);
int				count_consecutive_backslashes(const char *text, int *i);
t_fragment		*handle_backslashes_dispatch(int count, const char *text,
					int *i);
t_fragment		*handle_spaces(const char *text, int *i);
t_fragment		*handle_dollar_string_lexer(const char *text, int *i);
t_fragment		*make_dollar_fragment(const char *text,
					int start, int len, char quote);
bool			calc_space_after(const char *text, int i);
t_fragment		*handle_single_quotes(const char *text, int *i);
t_fragment		*handle_double_quotes(const char *text, int *i);
t_fragment		*handle_operators(const char *text, int *i);
t_fragment		*handle_generic_text(const char *text, int *i);
t_fragment		*get_next_fragment(const char *text, int *i);

/* ********************************************************************* */
/*                                 Debug                                 */
/* ********************************************************************* */

void			print_fragments(t_fragment *fragments);
void			print_tokens_raw(t_token *tokens);

#endif
