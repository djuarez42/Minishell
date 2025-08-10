/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42barcelona.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 21:27:58 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/07 22:30:11 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"
//#include <stdio.h>
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	unsigned int	i;
	char			*result;

	if (s == NULL)
		return (NULL);
	i = 0;
	result = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (result == NULL)
		return (NULL);
	while (s[i] != '\0')
	{
		result[i] = (f)(i, s[i]);
		i++;
	}
	result[i] = '\0';
	return (result);
}
/*char	to_upper(unsigned int i, char c)
{
	i = 0;
	if (c >= 'a' && c  <= 'z')
		c = c - 32 + i;
	return (c);
}
int	main(void)
{
	char	*str;
	char	*result;
	
	str = "iolwmhhdmdkejeksksjenn";
	result =ft_strmapi(str, to_upper);
	printf("the new string is %s\n", result);
	free(result);
	return (0);
}*/
