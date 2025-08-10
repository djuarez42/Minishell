/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 17:59:29 by djuarez           #+#    #+#             */
/*   Updated: 2024/06/25 19:06:00 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stddef.h> // Necesario para size_t
//#include <stdio.h>
#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	unsigned char	*str;
	unsigned char	uc;
	char			*res;
	size_t			i;

	str = (unsigned char *)s;
	uc = (unsigned char)c;
	res = NULL;
	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == uc)
			res = (char *)&str[i];
		i++;
	}
	if (str[i] == uc)
		res = (char *)&str[i];
	return (res);
}
/*
int	main (void)
{
	const char	*s = "hola mundo";
	char	caracter = 'n';
	char	*resultado = ft_strrchr(s, caracter);

	if (resultado != NULL)
		printf("ultima ocurrencia de '%c': %s\n", caracter, resultado);
	else
		printf("Caracter '%c' no enco trado en la cadena. \n", caracter);

	return (0);
}*/
