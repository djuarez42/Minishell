/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:28:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 19:36:32 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <stdlib.h>
# include <stdio.h>
# include "libft.h"
# include <stdbool.h>

/* ---------------------------- Token types ---------------------------- */
typedef enum e_token_type
{
	TOKEN_WORD,         // normal word   #0
	TOKEN_PIPE,         // pipe '|'      #1
	TOKEN_REDIRECT_IN,  // <             #2
	TOKEN_REDIRECT_OUT, // >             #3
	TOKEN_HEREDOC,      // <<            #4
	TOKEN_APPEND,       // >>            #5
	TOKEN_EOF,          // end of file   #6
	TOKEN_IN_FILE,      // Input file    #7
	TOKEN_OUT_FILE      // Output file   #8
}	t_token_type;

/* ---------------------------- Quote types ---------------------------- */
typedef enum e_quote_type
{
	QUOTE_NONE,    // #0
	QUOTE_SINGLE,  // #1
	QUOTE_DOUBLE,  // #2
	QUOTE_MIXED,   // #3
	QUOTE_DOLLAR   // #4
}	t_quote_type;

/* ---------------------------- Structures ----------------------------- */
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

/* ----------------------- Structure management ------------------------ */
t_token		*create_token(t_token_type type, bool has_space_before);
t_fragment	*new_fragment(const char *text, size_t len,
				t_quote_type quote_type, bool has_space_after);
char		*concat_fragments(t_fragment *fragments);
void		append_fragment(t_fragment **list, t_fragment *new_frag);
void		free_fragments(t_fragment *frag);
void		free_token_list(t_token *tokens);

/* ------------------------ Tokenization core -------------------------- */
t_token		*tokenize_input(const char *input);
t_token_type	determine_token_type(char *str, t_quote_type quote);
t_token		*append_token_eof(t_token *head);

/* -------------------------- Quote helpers ---------------------------- */
char		*remove_quotes(char *str);
int			check_unmatched_quotes(const char *input);
t_fragment	*parse_mixed_fragments(const char *text);
t_fragment	*handle_backslashes(const char *text, int *i);

/* ----------------------------- Debug -------------------------------- */
void		print_fragments(t_fragment *fragments);
void		print_tokens_raw(t_token *tokens);

#endif
