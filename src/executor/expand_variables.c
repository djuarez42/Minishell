/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 22:39:16 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*ft_strdupc(char c)
{
	char	*str;

	str = (char *)malloc(sizeof(char) * 2);
	if (!str)
		return (NULL);
	str[0] = c;
	str[1] = '\0';
	return (str);
}

static char	*handle_dollar_string(const char *input, int *i)
{
	char	*result;
	int		start;
	int		end;

	if (!input)
		return (ft_strdup(""));
	(*i)++;
	if (input[*i] == '"')
		(*i)++;
	start = *i;
	end = start;
	while (input[end] && input[end] != '"')
		end++;
	result = ft_substr(input, start, end - start);
	if (!result)
	{
		return (NULL);
	}
	if (input[end] == '"')
		*i = end + 1;
	else
		*i = end;
	return (result);
}

int	is_dollar_string(const char *input, int pos)
{
	if (!input || !input[pos])
		return (0);
	return (input[pos] == '$' && input[pos + 1] && (input[pos + 1] == '"'
			|| input[pos + 1] == '\''));
}
static char	*handle_dollar(const char *input, int *i, t_dollar_ctx ctx)
{
	if (is_dollar_string(input, *i))
		return (handle_dollar_string(input, i));
	if (ctx.quote == QUOTE_DOUBLE || ctx.quote == QUOTE_NONE)
		return (handle_dollar_quotes_fix(input, i, ctx.envp, ctx.state));
	(*i)++;
	return (ft_strdup("$"));
}

static int	append_piece(char **acc, char *piece)
{
	char	*tmp;

	tmp = str_append(*acc, piece);
	free(piece);
	*acc = tmp;
	return (*acc == NULL);
}

char	*expand_variables(const char *input, char **envp, t_exec_state *state,
			t_quote_type quote)
{
	int		i;
	char	*tmp;
	char	*piece;
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
		if (input[i] == '\\' && input[i + 1] == '$')
		{
			piece = ft_strdup("\\$");
			i += 2;
		}
		else if (input[i] == '$')
			piece = handle_dollar(input, &i, ctx);
		else
			piece = ft_strdupc(input[i++]);
		if (!piece || append_piece(&tmp, piece))
			return (free(tmp), NULL);
	}
	return (tmp);
}
