/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils9.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 02:34:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 16:59:04 by ekakhmad         ###   ########.fr       */
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

static char	*append_char_or_escape(const char *input, int *i, char *tmp,
		int is_backslash)
{
	char	buf[2];

	if (is_backslash && input[*i + 1])
	{
		buf[0] = input[*i + 1];
		buf[1] = '\0';
		tmp = str_append(tmp, buf);
		*i += 2;
		return (tmp);
	}
	if (is_backslash)
		buf[0] = '\\';
	else
		buf[0] = input[*i];
	buf[1] = '\0';
	tmp = str_append(tmp, buf);
	(*i)++;
	return (tmp);
}

static char	*process_expansion_char(const char *input, int *i,
		char *tmp, t_dollar_ctx *ctx)
{
	if (input[*i] == 1 && input[*i + 1] == '$')
	{
		tmp = str_append(tmp, "$");
		*i += 2;
	}
	else if (input[*i] == '\\' && ctx->quote == QUOTE_DOUBLE)
		tmp = append_char_or_escape(input, i, tmp, 1);
	else if (input[*i] == '$')
		tmp = handle_dollar(input, i, tmp, ctx);
	else if (input[*i] >= 32 || input[*i] == '\t' || input[*i] == '\n')
		tmp = append_char_or_escape(input, i, tmp, 0);
	else
		(*i)++;
	return (tmp);
}

char	*expand_variables(const char *input, char **envp,
	t_exec_state *state, t_quote_type quote)
{
	int				i;
	char			*tmp;
	t_dollar_ctx	ctx;

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
		tmp = process_expansion_char(input, &i, tmp, &ctx);
		if (!tmp)
			return (NULL);
	}
	return (tmp);
}
