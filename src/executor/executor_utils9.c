/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils9.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 02:34:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/02 17:14:30 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_dollar_string(const char *input, int pos)
{
	if (!input || !input[pos])
		return (0);
	return (input[pos] == '$' && input[pos + 1] && (input[pos + 1] == '"'
			|| input[pos + 1] == '\''));
}

/* handle_escaped_dollar removed: parity-aware handling implemented inline in
	expand_variables (counts backslashes and decides whether to expand $ or
	emit a literal '$'). */

static char	*handle_dollar(const char *input, int *i,
		char *tmp, t_dollar_ctx *ctx)
{
	char	*piece;

	if (is_dollar_string(input, *i))
		piece = handle_dollar_string(input, i);
	else if (ctx->quote == QUOTE_DOUBLE || ctx->quote == QUOTE_NONE)
		piece = handle_dollar_quotes_fix(input, i, ctx->envp, ctx->state);
	else
	{
		piece = ft_strdup("$");
		(*i)++;
	}
	tmp = str_append(tmp, piece);
	free(piece);
	return (tmp);
}

static char	*handle_literal(const char *input, int *i, char *tmp)
{
	char	*piece;

	piece = (char *)malloc(2);
	if (!piece)
		return (NULL);
	piece[0] = input[*i];
	piece[1] = '\0';
	tmp = str_append(tmp, piece);
	free(piece);
	(*i)++;
	return (tmp);
}

char	*expand_variables(const char *input, char **envp,
				t_exec_state *state, t_quote_type quote)
{
	int				i;
	char			*tmp;
	t_dollar_ctx	ctx;
	/* parity temporaries removed */

	if (!input)
		return (NULL);
	if (input[0] == '\0')
		return (ft_strdup(""));
	i = 0;
	tmp = NULL;
	ctx.envp = envp;
	ctx.state = state;
	ctx.quote = quote;
	while (input[i])
	{
		if (input[i] == '\\')
		{
			/* simple behavior: if next char exists, consume the backslash
			   and append the next char literally (escape); otherwise emit
			   the backslash */
			if (input[i + 1])
			{
				char esc[2];

				esc[0] = input[i + 1];
				esc[1] = '\0';
				tmp = str_append(tmp, esc);
				i += 2;
				if (!tmp)
					return (NULL);
				continue;
			}
			/* trailing backslash */
			tmp = str_append(tmp, "\\");
			i++;
			if (!tmp)
				return (NULL);
			continue;
		}
		else if (input[i] == '$')
			tmp = handle_dollar(input, &i, tmp, &ctx);
		else
			tmp = handle_literal(input, &i, tmp);
		if (!tmp)
			return (NULL);
	}
	return (tmp);
}
