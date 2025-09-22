/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils8.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:43:00 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/22 03:07:35 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*handle_dollar_string(const char *input, int *i)
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
		return (NULL);
	if (input[end] == '"')
		*i = end + 1;
	else
		*i = end;
	return (result);
}

static char	interpret_single_escape(char c)
{
	if (c == 'n')
		return ('\n');
	if (c == 't')
		return ('\t');
	if (c == 'r')
		return ('\r');
	if (c == '\\')
		return ('\\');
	if (c == '\'')
		return ('\'');
	if (c == '"')
		return ('"');
	return (c);
}

static void	handle_escape(const char *str, int *i, char *res, int *j)
{
	(*i)++;
	res[*j] = interpret_single_escape(str[*i]);
	(*j)++;
	(*i)++;
}

char	*interpret_ansi_c_escapes(const char *str)
{
	int		i;
	int		j;
	char	*res;
	size_t	len;

	i = 0;
	j = 0;
	len = ft_strlen(str);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	while (str[i])
	{
		if (str[i] == '\\' && str[i + 1])
			handle_escape(str, &i, res, &j);
		else
			res[j++] = str[i++];
	}
	res[j] = '\0';
	return (res);
}

char	*expand_ansi_c_string(const char *input)
{
	char	*res;
	char	*tmp;

	if (!input)
		return (ft_strdup(""));
	res = ft_strdup(input);
	if (!res)
		return (NULL);
	tmp = res;
	res = interpret_ansi_c_escapes(res);
	free(tmp);
	return (res);
}
