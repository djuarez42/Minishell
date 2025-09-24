/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils5.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:12:31 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 17:15:14 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*dup_name(const char *env)
{
	const char	*eq;
	size_t		len;

	eq = ft_strchr(env, '=');
	if (!eq)
		return (ft_strdup(env));
	len = eq - env;
	return (ft_strndup(env, len));
}
