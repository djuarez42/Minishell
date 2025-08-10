/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42barcelona.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:33:10 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/07 00:33:00 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
//#include <stdio.h>
static int	get_num_len(int n)
{
	int	len;

	len = 0;
	if (n != 0)
	{
		if (n < 0)
		{
			n *= -1;
			len++;
		}
		while (n != 0)
		{
			n /= 10;
			len++;
		}
	}
	else
		len = 1;
	return (len);
}

char	*ft_itoa(int n)
{
	int			len;
	char		*str;
	long int	nbr;

	len = get_num_len(n);
	nbr = n;
	str = (char *)malloc((len + 1) * sizeof(char));
	if (str == NULL)
		return (0);
	str[len--] = '\0';
	if (nbr == 0)
		str[0] = '0';
	if (nbr < 0)
	{
		str[0] = '-';
		nbr = -nbr;
	}
	while (nbr != 0)
	{
		str[len] = (nbr % 10) + '0';
		nbr /= 10;
		len--;
	}
	return (str);
}
/*int main()
{
	int	number;
	char *number_str;

	number = -225533;
	number_str = ft_itoa(number);

	if (number_str != NULL)
	{
	printf("el numero asignado es: %s\n", number_str);
	free(number_str);
	}
	else
	{
			printf("error al convertir el numero.\n");
	}
			return(0);
}*/
