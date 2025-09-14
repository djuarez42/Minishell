/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 16:45:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 00:12:41 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"


char	*handle_quoted_part(const char *input, int *i, char *tmp,
	t_quote_type *last_quote)
{
	char	*segment;
	int		len;
	char	current_quote;

	current_quote = input[*i];
	
	// For quotes containing a single opposite quote character, treat as literal
	if (current_quote == '\"' && input[*i + 1] == '\'' && input[*i + 2] == '\"')
	{
		// This is a pattern like "'" - should output literal '
		tmp = str_append(tmp, "'");
		*i += 3; // Skip past "'"
		*last_quote = QUOTE_DOUBLE;
		return (tmp);
	}
	else if (current_quote == '\'' && input[*i + 1] == '\"' && input[*i + 2] == '\'')
	{
		// This is a pattern like '"' - should output literal "
		tmp = str_append(tmp, "\"");
		*i += 3; // Skip past '"'
		*last_quote = QUOTE_SINGLE;
		return (tmp);
	}
	
	// Regular quote handling
	// Set the quote type based on the quote character
	if (current_quote == '\'')
		*last_quote = QUOTE_SINGLE;
	else if (current_quote == '\"')
		*last_quote = QUOTE_DOUBLE;
	
	// Extract the content inside the quotes
	segment = extract_quoted_segment(&input[*i], &len);
	if (!segment)
		return (tmp);

	// Append the segment to the result
	tmp = str_append(tmp, segment);
	free(segment);
	*i += len;
	return (tmp);
}


char	*extract_quoted_segment(const char *input, int *len)
{
	int		i;
	int		j;
	char	quote;
	char	*result;

	if (!input || !is_quote(*input))
		return (NULL);
	quote = *input;
	i = 1;
	while (input[i] && input[i] != quote)
		i++;
	if (!input[i])
		return (NULL);
	*len = i + 1;
	// Allocate exact space needed (i-1 for content + 1 for null)
	result = malloc(i);
	if (!result)
		return (NULL);
	j = 0;
	// Copy content inside quotes exactly as is, preserving all spaces
	while (j < i - 1)
	{
		result[j] = input[j + 1];
		j++;
	}
	result[j] = '\0';
	return (result);
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
