/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:09:54 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/07 16:15:05 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"

char	*handle_quoted_part(const char *input, int *i, char *tmp)
{
	int		len;
	char	*segment;

	segment = extract_quoted_segment(&input[*i], &len);
	if (!segment)
	{
		printf("❌ extract_quoted_segment falló en i = %d\n", *i);
		*i += 1;
		return (tmp);
	}
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

char	**reconstruct_words(const char *input)
{
	int		i;
	int		last_i;
	int		tok_i;
	char	*tmp;
	char	**tokens;

	i = 0;
	tok_i = 0;
	tmp = NULL;
	tokens = malloc(sizeof(char *) * 1024);
	if (!tokens)
		return (NULL);
	while (input[i])
	{
		last_i = i;
		i = process_spaces_and_quotes(input, i, &tmp);
		if (i == -1)
			break ;
		if (should_add_token(input, i, tmp))
			check_and_add_token(tokens, &tok_i, &tmp);
		if (last_i == i)
			i++;
	}
	tokens[tok_i] = NULL;
	return (tokens);
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
		// Handle quote state
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
		// Check for comment start (# not in quotes)
		else if (input[i] == '#' && quote_state == 0)
		{
			// Check if # is at start of line or after whitespace
			if (i == 0 || ft_isspace(input[i - 1]))
			{
				// Found comment - terminate string here
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

char	**clean_input_quotes(const char *input)
{
	char	*comment_free_input;
	char	**result;

	if (!input)
		return (NULL);
	
	// First strip comments
	comment_free_input = strip_comments(input);
	if (!comment_free_input)
		return (NULL);
	
	// Then check quotes and reconstruct words
	if (!are_quotes_closed(comment_free_input))
	{
		free(comment_free_input);
		return (NULL);
	}
	result = reconstruct_words(comment_free_input);
	free(comment_free_input);
	return (result);
}
