/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset_exit.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:50:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/23 20:25:34 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "libft.h"

int	bi_unset(char **argv, char ***penvp)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		// Check if variable is valid (already expanded by the parser)
		if (env_identifier_valid(argv[i]))
			(void)env_unset_var(penvp, argv[i]);
		else
			fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", argv[i]);
		i++;
	}
	return (0);
}

int	bi_exit(char **argv)
{
	int	status;

	status = 0;
	if (argv[1])
		status = ft_atoi(argv[1]);
	return (status & 0xFF);
}
