/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_free.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 12:48:34 by djuarez           #+#    #+#             */
/*   Updated: 2025/07/28 17:03:33 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"
#include <stdio.h>

char	*ft_strjoin_free(char *s1, const char *s2)
{
	char	*joined;

	if (!s1)
	{
		if (!s2)
		{
			return (NULL);
		}
		return (ft_strdup(s2));
	}
	if (!s2)
	{
		return (NULL);
	}
	joined = ft_strjoin(s1, s2);
	if (!joined)
	{
		free(s1);
		return (NULL);
	}
	free(s1);
	return (joined);
}
