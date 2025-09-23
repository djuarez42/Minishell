/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils11.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 19:18:28 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 16:00:59 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*remove_all_quotes(const char *s)
{
	size_t	len;
	size_t	i;
	size_t	j;
	char	*out;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	while (i < len)
	{
		if (s[i] != '\'' && s[i] != '"')
			out[j++] = s[i];
		i++;
	}
	out[j] = '\0';
	return (out);
}

char	*create_backslash_string(int count)
{
	char	*result;
	int		i;

	if (count <= 0)
		return (ft_strdup(""));
	result = malloc(count + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (i < count)
	{
		result[i] = '\\';
		i++;
	}
	result[count] = '\0';
	return (result);
}

static void	handle_backslash_sequence(const char *input, int *i, int *count)
{
	*count = 0;
	while (input[*i] == '\\')
	{
		(*count)++;
		(*i)++;
	}
}

static void	handle_dollar_case(int *i, int backslash_count,
								int *backslashes_out, bool *should_expand)
{
	if (backslash_count % 2 == 0)
	{
		*backslashes_out = backslash_count / 2;
		*should_expand = true;
		(*i)++;
	}
	else
	{
		*backslashes_out = backslash_count / 2;
		*should_expand = false;
		(*i)--;
	}
}

void	handle_backslash_dollar_parity(const char *input, int *i,
										int *backslashes_out,
										bool *should_expand)
{
	int	backslash_count;

	handle_backslash_sequence(input, i, &backslash_count);
	if (input[*i] == '$')
		handle_dollar_case(i, backslash_count, backslashes_out, should_expand);
	else
	{
		*backslashes_out = backslash_count;
		*should_expand = false;
	}
}
