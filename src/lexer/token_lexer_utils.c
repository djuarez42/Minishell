/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_lexer_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:06:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/25 20:30:51 by djuarez          ###   ########.fr       */
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

int	should_add_token(const char *input, int i)
{
	if (input[i] == '\0' || ft_isspace(input[i]))
		return (1);
	return (0);
}

void	check_and_add_token(char **tokens, int *tok_i, char **tmp)
{
	if (!*tmp)
		*tmp = ft_strdup("");
	add_token(tokens, tok_i, tmp);
}

int	process_spaces_and_quotes(const char *input, int i, char **tmp,
		t_quote_type *last_quote)
{
	i = skip_spaces(input, i);
	if (!input[i])
		return (-1);
	if (is_quote(input[i]))
		*tmp = handle_quoted_part(input, &i, *tmp, last_quote);
	else if (!ft_isspace(input[i]) && input[i] != '\0')
		*tmp = handle_plain_text(input, &i, *tmp);
	return (i);
}

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
