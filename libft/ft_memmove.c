/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42barcelona.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 13:47:28 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/16 14:00:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	void	*dest_ptr;

	dest_ptr = dest;
	if (!dest && !src)
		return (NULL);
	if (dest == src)
		return (dest);
	if (dest > src)
	{
		while (n--)
		{
			((char *)dest)[n] = ((const char *)src)[n];
		}
	}
	else
	{
		while (n--)
		{
			*(char *)dest++ = *(const char *)src++;
		}
	}
	return (dest_ptr);
}
/*#include <stdio.h> // Para printf
#include <string.h> // Para memmove
int main () 
{
   char dest[] = "Ra";
   const char src[]  = "Hello";

   memmove(dest, src, 3);
   printf("After memmove dest = %s, src = %s\n", dest, src);
   ft_memmove(dest, src, 3);
   printf("After ft_memmove dest = %s, src = %s\n", dest, src);
   return(0);
}*/
