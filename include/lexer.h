/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:28:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/25 20:32:07 by djuarez          ###   ########.fr       */
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

//Token structure
typedef struct s_token
{
	t_token_type	type;
	t_quote_type	quote_type;
	char			*value;
	struct s_token	*next;
}	t_token;

//reconstruct_words
typedef struct s_reconstruct
{
	int				i;
	int				last_i;
	int				tok_i;
	char			*tmp;
	char			**tokens;
	t_quote_type	token_quote;
}	t_reconstruct;

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
t_token			*append_token_eof(t_token *head, t_token *cur);
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

#endif
