/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 22:13:52 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/06 18:15:26 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memset(void *s, int c, size_t n)
{
	size_t				i;
	unsigned char		*tmp_s;

	i = 0;
	tmp_s = (unsigned char *)s;
	while (i < n)
	{
		tmp_s[i] = (unsigned char)c;
		i++;
	}
	return (tmp_s);
}
