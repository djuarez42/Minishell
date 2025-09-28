/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_level_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 18:20:13 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/28 17:38:46 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*make_shlvl_str(int lvl)
{
	char	*number;
	char	*result;

	number = ft_itoa(lvl);
	if (!number)
		return (NULL);
	result = malloc(ft_strlen("SHLVL=") + ft_strlen(number) + 1);
	if (!result)
	{
		free(number);
		return (NULL);
	}
	ft_strlcpy(result, "SHLVL=", ft_strlen("SHLVL=") + 1);
	ft_strlcat(result, number, ft_strlen("SHLVL=") + ft_strlen(number) + 1);
	free(number);
	return (result);
}

void	update_shlvl(char **envp)
{
	int		i;
	int		lvl;
	char	*new_val;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "SHLVL=", 6) == 0)
		{
			lvl = ft_atoi(envp[i] + 6);
			lvl++;
			new_val = make_shlvl_str(lvl);
			if (!new_val)
				return ;
			free(envp[i]);
			envp[i] = new_val;
			return ;
		}
		i++;
	}
	envp[i] = ft_strdup("SHLVL=1");
	envp[i + 1] = NULL;
}
