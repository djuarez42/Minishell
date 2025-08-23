/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 16:09:54 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/23 12:07:26 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"

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

static void	update_quote_state(char c, int *quote_state, char *open_quote)
{
	if (*quote_state == 0 && (c == '"' || c == '\''))
	{
		*quote_state = 1;
		*open_quote = c;
	}
	else if (*quote_state == 1 && c == *open_quote)
	{
		*quote_state = 0;
		*open_quote = 0;
	}
}

static int	is_comment_here(const char *input, int i, int quote_state)
{
	return (quote_state == 0 && input[i] == '#'
		&& (i == 0 || ft_isspace(input[i - 1])));
}

static int	copy_or_stop(const char *input, char *result,
			int i, int quote_state)
{
	if (is_comment_here(input, i, quote_state))
	{
		result[i] = '\0';
		return (0);
	}
	result[i] = input[i];
	return (1);
}

char	*strip_comments(const char *input)
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
		update_quote_state(input[i], &quote_state, &open_quote);
		if (!copy_or_stop(input, result, i, quote_state))
			break ;
		i++;
	}
	result[i] = '\0';
	return (result);
}
