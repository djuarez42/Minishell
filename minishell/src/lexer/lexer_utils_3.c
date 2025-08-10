/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 16:45:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/06 21:18:08 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

const char	*token_type_str(t_token_type type)
{
	if (type == TOKEN_WORD)
		return ("WORD");
	if (type == TOKEN_PIPE)
		return ("PIPE");
	if (type == TOKEN_REDIRECT_IN)
		return ("REDIRECT_IN");
	if (type == TOKEN_REDIRECT_OUT)
		return ("REDIRECT_OUT");
	if (type == TOKEN_HEREDOC)
		return ("HEREDOC");
	if (type == TOKEN_APPEND)
		return ("APPEND");
	if (type == TOKEN_EOF)
		return ("EOF");
	return ("UNKNOWN");
}

char	*remove_quotes(char *str)
{
	int	len;

	len = ft_strlen(str);
	if (len > 1 && str[0] == '"' && str[len - 1] == '"')
	{
		str[len - 1] = '\0';
		return (str + 1);
	}
	return (str);
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
