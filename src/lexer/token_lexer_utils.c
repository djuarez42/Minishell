/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_lexer_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:06:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/07 16:11:57 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "minishell.h"

int	token_len(const char *s)
{
	int		i;
	char	quote;

	i = 0;
	if (is_quote(s[0]))
	{
		quote = s[0];
		i++;
		while (s[i] && s[i] != quote)
			i++;
		if (s[i] == quote)
			i++;
	}
	else
	{
		while (s[i] && !is_operator(s[i]) && !is_quote(s[i])
			&& !ft_isspace(s[i]))
			i++;
	}
	return (i);
}

int	should_add_token(const char *input, int i, char *tmp)
{
	if (!tmp)
		return (0);
	if (input[i] == '\0' || ft_isspace(input[i]))
		return (tmp[0] != '\0');
	return (0);
}

void	check_and_add_token(char **tokens, int *tok_i, char **tmp)
{
	add_token(tokens, tok_i, tmp);
}

int	process_spaces_and_quotes(const char *input, int i, char **tmp)
{
	i = skip_spaces(input, i);
	if (!input[i])
		return (-1);
	if (is_quote(input[i]))
		*tmp = handle_quoted_part(input, &i, *tmp);
	else if (!ft_isspace(input[i]) && input[i] != '\0')
		*tmp = handle_plain_text(input, &i, *tmp);
	return (i);
}
