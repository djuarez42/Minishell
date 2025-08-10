/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/19 22:32:30 by djuarez           #+#    #+#             */
/*   Updated: 2024/06/19 22:55:43 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int		i;
	char	*ptr;

	i = 0;
	ptr = (char *)s;
	while (ptr[i] != '\0' && (ptr[i] != (unsigned char)c))
		i++;
	if (ptr[i] == (unsigned char)c)
		return (&ptr[i]);
	return (NULL);
}
