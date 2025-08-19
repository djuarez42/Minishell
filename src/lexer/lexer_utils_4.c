/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:09:54 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/19 19:52:46 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"

char	*handle_quoted_part(const char *input, int *i, char *tmp,
			t_quote_type *last_quote)
{
	int		len;
	char	*segment;
    char    quote;

	if (input[*i] == '\'')
		*last_quote = QUOTE_SINGLE;
	else if (input[*i] == '\"')
		*last_quote = QUOTE_DOUBLE;
	segment = extract_quoted_segment(&input[*i], &len);
	if (!segment)
	{
		printf("❌ extract_quoted_segment falló en i = %d\n", *i);
		*i += 1;
		return (tmp);
	}
	// Preserve double quotes content; treat single quotes similarly (no escape handling for now)
	quote = input[*i];
	(void)quote;
	tmp = str_append(tmp, segment);
	free (segment);
	*i += len;
	return (tmp);
}

char	*handle_plain_text(const char *input, int *i, char *tmp)
{
	int		start;
	char	*segment;

	start = *i;
	while (input[*i] && !is_quote(input[*i]) && !ft_isspace(input[*i]))
		(*i)++;
	segment = ft_substr(input, start, *i - start);
	tmp = str_append(tmp, segment);
	free(segment);
	return (tmp);
}

void	add_token(char **tokens, int *tok_i, char **tmp)
{
	tokens[*tok_i] = *tmp;
	(*tok_i)++;
	*tmp = NULL;
}

char	**reconstruct_words(const char *input, t_quote_type *last_quote,
			t_quote_type **quotes_out)
{
	t_reconstruct	r;

	if (!init_tokens_and_quotes(&r.tokens, quotes_out))
		return (NULL);
	r.i = 0;
	r.tok_i = 0;
	r.tmp = NULL;
	while (input[r.i])
	{
		r.last_i = r.i;
		r.i = process_spaces_and_quotes(input, r.i, &r.tmp, last_quote);
		if (r.i == -1)
			break ;
		if (should_add_token(input, r.i, r.tmp))
		{
			r.token_quote = *last_quote;
			check_and_add_token(r.tokens, &r.tok_i, &r.tmp);
			(*quotes_out)[r.tok_i - 1] = r.token_quote;
		}
		if (r.last_i == r.i)
			r.i++;
	}
	r.tokens[r.tok_i] = NULL;
	(*quotes_out)[r.tok_i] = QUOTE_NONE;
	return (r.tokens);
}

int	init_tokens_and_quotes(char ***tokens_out, t_quote_type **quotes_out)
{
	*tokens_out = malloc(sizeof(char *) * 1024);
	if (!*tokens_out)
		return (0);
	*quotes_out = malloc(sizeof(t_quote_type) * 1024);
	if (!*quotes_out)
	{
		free(*tokens_out);
		return (0);
	}
	return (1);
}
// Helper to strip comments from input line
// Comments start with # when not inside quotes and after whitespace or at start of line
static char	*strip_comments(const char *input)
{
	int		i;
	int		quote_state;
	char	open_quote;
	char	*result;
	int		len;

	if (!input)
		return (NULL);
	len = ft_strlen(input);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	i = 0;
	quote_state = 0;
	open_quote = 0;
	while (input[i])
	{
		if (is_quote(input[i]) && quote_state == 0)
		{
			quote_state = 1;
			open_quote = input[i];
		}
		else if (quote_state == 1 && input[i] == open_quote)
		{
			quote_state = 0;
			open_quote = 0;
		}
		else if (input[i] == '#' && quote_state == 0)
		{
			if (i == 0 || ft_isspace(input[i - 1]))
			{
				result[i] = '\0';
				return (result);
			}
		}
		result[i] = input[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

char	**clean_input_quotes(const char *input, t_quote_type **quotes_out)
{
	char			*comment_free_input;
	char			**result;
	t_quote_type	last_quote;

	if (!input)
		return (NULL);
	comment_free_input = strip_comments(input);
	if (!comment_free_input)
		return (NULL);
	last_quote = QUOTE_NONE;
	if (!are_quotes_closed(comment_free_input))
	{
		free(comment_free_input);
		return (NULL);
	}
	result = reconstruct_words(comment_free_input, &last_quote, quotes_out);
	free(comment_free_input);
	return (result);
}
