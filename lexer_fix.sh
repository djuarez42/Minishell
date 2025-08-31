#!/bin/bash

# Fix implementation for tokenizer to handle dollar-dollar cases
echo "Implementing lexer fix for variable expansion..."

# Modify lexer_utils_3.c
cat > src/lexer/lexer_utils_3.c << 'EOF'
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 16:45:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/30 20:00:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

/*
 * Handle quoted parts with improved dollar-sign handling
 */
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

char	*remove_quotes(char *str)
{
	int		len;
	char	*result;

	len = ft_strlen(str);
	// Only remove double quotes, preserve single quotes
	if (len > 1 && str[0] == '"' && str[len - 1] == '"')
		result = ft_substr(str, 1, len - 2);
	else
		result = ft_strdup(str);
	return (result);
}

bool	are_quotes_closed(const char *input)
{
	int		i;
	char	open_quote;

	i = 0;
	open_quote = 0;
	while (input[i])
	{
		if (is_quote(input[i]))
		{
			if (open_quote == 0)
				open_quote = input[i];
			else if (input[i] == open_quote)
				open_quote = 0;
		}
		i++;
	}
	return (open_quote == 0);
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
	result = malloc(i);
	if (!result)
		return (NULL);
	j = 0;
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
EOF

echo "Fix implemented. Rebuilding..."
make
