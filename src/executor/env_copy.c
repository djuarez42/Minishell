/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_copy.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:56:30 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/21 21:56:30 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "minishell.h"

static void	free_partial(char **copy, int i)
{
	while (i > 0)
		free(copy[--i]);
	free(copy);
}

char	**new_envp(char **envp)
{
	int		count;
	int		i;
	char	**copy;

	count = 0;
	while (envp[count])
		count++;
	copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]);
		if (!copy[i])
			return (free_partial(copy, i), NULL);
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

void	free_envp(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
		free(envp[i++]);
	free(envp);
}
