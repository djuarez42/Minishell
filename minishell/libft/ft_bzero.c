/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 20:55:54 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/06 17:21:53 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

void	ft_bzero(void *s, size_t n)
{
	size_t			i;
	unsigned char	*p;

	i = 0;
	p = (unsigned char *)s;
	while (i < n)
	{
		p[i] = '\0';
		i++;
	}
}
/*int main ()
{
	char	str[20] = "Hello, World!";

	printf("original chain: '%s'\n", str);

	ft_bzero(str, 5);

	printf("Cadena despues de ft_bzero: '%s'\n", str);
	printf("cadena en hexadecimal: ");
	for (size_t i = 0; i <sizeof(str); i++)
	{
		printf("%02x ", (unsigned char)str[i]);
	}
	printf("\n");
}*/
