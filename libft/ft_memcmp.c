/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 22:36:27 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/16 14:19:39 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char	*str1;
	unsigned char	*str2;
	size_t			i;

	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	i = 0;
	while (i < n)
	{
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
		i++;
	}
	return (0);
}
/*#include <stdio.h>
#include <string.h>
int main(void)
{
	char str1[] = "abcdef";
	char str2[] = "abcde333g";
	size_t n = 10;
	int result;

	result = ft_memcmp(str1, str2, n);

	printf("Comparing '%s' and '%s' up to %zu bytes:\n", str1, str2, n);
	if (result < 0)
		printf("str1 is less than str2.\n");
	else if (result > 0)
		printf("str1 is greater than  str2. \n");
	else
		printf("str1 is equal to str2. \n");
	return (0);
}*/
