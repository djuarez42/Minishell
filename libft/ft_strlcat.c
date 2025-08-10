/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42barcelona.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 20:57:51 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/17 20:51:07 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
#include "libft.h"
#include <unistd.h>

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	lendst;
	size_t	lensrc;
	size_t	lentotal;

	lendst = 0;
	lensrc = 0;
	i = 0;
	while (dst[lendst] != '\0' && lendst < size)
		lendst++;
	while (src[lensrc])
		lensrc++;
	lentotal = lendst + lensrc;
	if (lendst >= size)
		return (size + lensrc);
	while (src[i] != '\0' && (i < (size - lendst - 1)))
	{
		dst[lendst + i] = src[i];
		i++;
	}
	dst[lendst + i] = '\0';
	return (lentotal);
}
//Len total es igual a lendst + lensrc que es lo que devuelve la funcion
//la funcion en tambien copia caracteres de src a destination asegurandose
//de no exeder el tamano de size que es el numero que garantiza que no se 
//desborde el tamano de dst especificado por size.
//
// while (src[i] != '\0' && (i < (size - lendst - 1)))
// {
//     dst[lendst + i] = src[i];
//     i++;
// }
// pero si lendest es mayou o igual que size significa que dst esta lleno o
// excede su capacidad maxima. En este caso la funcion simplemente devuelve
// un valor que indica cuanto seria la longitud total de la cadena resultante
// si se pudiera concatenar completamente a src y la funcion devuelve 
// size + lensrc
/*int main(void)
{
	char	dst[50];
	strcpy(dst, "Hola me llamo");
	const char	*src;
	size_t	lendst_before;
	size_t	size;
	size_t	lentotal;

	src = " Cristhian";
	size = 50;
	lentotal = ft_strlcat(dst, src, size);
	lendst_before = strlen(src);
	printf("la cadena contatenada tiene un len de: '%zu'\n", lentotal);
	printf("la cadena sin concatenar es de: '%zu'\n", lendst_before);
	return (0);

}*/
