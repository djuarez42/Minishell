/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 18:27:29 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 21:53:32 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_fragments(t_fragment *frag)
{
	t_fragment	*tmp;

	while (frag)
	{
		tmp = frag->next;
		if (frag->text)
			free(frag->text);
		if (frag->expanded_text)
			free(frag->expanded_text);
		free(frag);
		frag = tmp;
	}
}

void	free_token_list(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		if (tokens->fragments)
			free_fragments(tokens->fragments);
		if (tokens->final_text)
			free(tokens->final_text);
		free(tokens);
		tokens = tmp;
	}
}

t_token_type	determine_token_type(char *str, t_quote_type quote)
{
	if (!str)
		return (TOKEN_WORD);
	if (quote != QUOTE_NONE)
		return (TOKEN_WORD);
	if (str[0] == '<' && str[1] == '<' && str[2] == '\0')
		return (TOKEN_HEREDOC);
	else if (str[0] == '>' && str[1] == '>' && str[2] == '\0')
		return (TOKEN_APPEND);
	else if (str[0] == '<' && str[1] == '\0')
		return (TOKEN_REDIRECT_IN);
	else if (str[0] == '>' && str[1] == '\0')
		return (TOKEN_REDIRECT_OUT);
	else if (str[0] == '|' && str[1] == '\0')
		return (TOKEN_PIPE);
	else
		return (TOKEN_WORD);
}

int	check_unmatched_quotes(const char *input)
{
	int	single_open;
	int	double_open;
	int	i;

	single_open = 0;
	double_open = 0;
	i = 0;
	while (input[i])
	{
		if (input[i] == '\'' && double_open == 0)
			single_open = !single_open;
		else if (input[i] == '"' && single_open == 0)
			double_open = !double_open;
		i++;
	}
	if (single_open || double_open)
	{
		fprintf(stderr, "minishell: syntax error: unmatched quotes\n");
		return (1);
	}
	return (0);
}
