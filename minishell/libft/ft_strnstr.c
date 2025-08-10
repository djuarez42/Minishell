/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 23:31:50 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/06 21:56:42 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"
#include <stdio.h>
#include <stddef.h>

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	if (big == NULL)
		return (0);
	if (little[0] == '\0')
		return ((char *)big);
	i = 0;
	while (big[i] != '\0')
	{
		j = 0;
		while (big[i + j] == little[j] && (i + j) < len)
		{
			if (little[j + 1] == '\0')
				return ((char *)big + i);
			j++;
		}
		i++;
	}
	return (0);
}
/*int main()
{
	const char	*largestring;
	const char	*smallstring;
	char	*function_creation;
	

	largestring = "hola me llamo cristhian";
	smallstring = "llamo";
	function_creation = ft_strnstr(largestring, smallstring, 17);
	if (function_creation)
		printf("la cadena encontrada es: %s,\n", function_creation);
	else
		printf("la cadena no se encontro \n");
	return (0);
}*/
