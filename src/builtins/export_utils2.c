/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:59:05 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 17:05:26 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	needs_quotes(const char *val)
{
	size_t	i;

	if (!val)
		return (0);
	i = 0;
	while (val[i])
	{
		if (!(ft_isalnum((unsigned char)val[i])
				|| val[i] == '_'
				|| val[i] == '/'
				|| val[i] == '.'
				|| val[i] == ':'
				|| val[i] == ','
				|| val[i] == '-'
				|| val[i] == '@'))
			return (1);
		i++;
	}
	return (0);
}

size_t	calc_extra_quotes(const char *val)
{
	size_t	i;
	size_t	extra;

	i = 0;
	extra = 0;
	while (val[i])
	{
		if (val[i] == '\'')
			extra += 3;
		i++;
	}
	return (extra);
}

static char	*alloc_quoted_buffer(const char *val, size_t extra)
{
	size_t	len;
	char	*out;

	len = ft_strlen(val);
	out = malloc(len + extra + 3);
	return (out);
}

static void	append_escaped_quote(char *out, size_t *pos)
{
	out[(*pos)++] = '\'';
	out[(*pos)++] = '\\';
	out[(*pos)++] = '\'';
	out[(*pos)++] = '\'';
}

char	*build_quoted_value(const char *val, size_t extra)
{
	size_t	i;
	size_t	pos;
	char	*out;

	out = alloc_quoted_buffer(val, extra);
	if (!out)
		return (NULL);
	pos = 0;
	out[pos++] = '\'';
	i = 0;
	while (val[i])
	{
		if (val[i] == '\'')
			append_escaped_quote(out, &pos);
		else
			out[pos++] = val[i];
		i++;
	}
	out[pos++] = '\'';
	out[pos] = '\0';
	return (out);
}
