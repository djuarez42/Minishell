/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_itoa_helpers.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: automated <auto@local>                      +#+  +:+      
	+#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 22:00:00 by automated         #+#    #+#             */
/*   Updated: 2025/09/27 22:00:00 by automated        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	compute_int_len(int n)
{
	int				len;
	unsigned int	nb;
	unsigned int	tmp;

	len = 0;
	if (n <= 0)
		len = 1;
	if (n < 0)
		nb = -n;
	else
		nb = n;
	tmp = nb;
	while (tmp > 0)
	{
		len++;
		tmp /= 10;
	}
	return (len);
}

void	fill_number_buffer(unsigned int nb, char *buffer, int len)
{
	int	i;

	i = len - 1;
	while (nb > 0)
	{
		buffer[i] = (nb % 10) + '0';
		nb /= 10;
		i--;
	}
}

void	itoa_buffer_int(int n, char *buffer)
{
	int				len;
	unsigned int	nb;

	len = compute_int_len(n);
	buffer[len] = '\0';
	if (n < 0)
	{
		buffer[0] = '-';
		nb = -n;
	}
	else if (n == 0)
	{
		buffer[0] = '0';
		nb = 0;
	}
	else
		nb = n;
	if (nb > 0)
		fill_number_buffer(nb, buffer, len);
}
