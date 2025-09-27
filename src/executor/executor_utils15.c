/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils15.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 21:00:15 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/27 20:02:12 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	compute_int_len(int n)
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

static void	fill_number_buffer(unsigned int nb, char *buffer, int len)
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

static void	itoa_buffer_int(int n, char *buffer)
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

int	open_heredoc_file(t_heredoc_args *args)
{
	int			fd;
	char		*path;
	static int	counter;
	char		count_str[16];

	counter = 0;
	itoa_buffer_int(getpid(), count_str);
	path = ft_strjoin("/tmp/.heredoc_", count_str);
	itoa_buffer_int(counter++, count_str);
	path = ft_strjoin_free(path, "_");
	path = ft_strjoin_free(path, count_str);
	args->heredoc_path = path;
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		free(path);
		itoa_buffer_int(getpid(), count_str);
		path = ft_strjoin(".heredoc_", count_str);
		itoa_buffer_int(counter++, count_str);
		path = ft_strjoin_free(ft_strjoin_free(path, "_"), count_str);
		args->heredoc_path = path;
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			print_errno("open heredoc");
			args->heredoc_path = NULL;
			return (-1);
		}
	}
	return (fd);
}
