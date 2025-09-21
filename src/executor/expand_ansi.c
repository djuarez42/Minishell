/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_ansi.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 22:58:43 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 22:58:43 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		{
			i++;
			if (str[i] == 'n')
				res[j++] = '\n';
			else if (str[i] == 't')
				res[j++] = '\t';
			else if (str[i] == 'r')
				res[j++] = '\r';
			else if (str[i] == '\\')
				res[j++] = '\\';
			else if (str[i] == '\'')
				res[j++] = '\'';
			else if (str[i] == '"')
				res[j++] = '"';
			else
				res[j++] = str[i];
			i++;
		}
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

