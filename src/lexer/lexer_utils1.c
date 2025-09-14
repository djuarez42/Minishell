/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 14:09:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 01:31:16 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*remove_quotes(char *str)
{
	int		len;
	char	*result;

	if (!str)
		return (NULL);

	len = ft_strlen(str);

	// Elimina solo comillas dobles externas
	if (len > 1 && str[0] == '"' && str[len - 1] == '"')
		result = ft_substr(str, 1, len - 2);
	else
		result = ft_strdup(str);

	return (result);
}

t_token *create_token(t_token_type type, bool space_before) 
{
    t_token *tok = malloc(sizeof(t_token));
    if (!tok)
        return NULL;

    tok->type = type;
    tok->fragments = NULL;
    tok->has_space_before = space_before;
    tok->final_text = NULL;
    tok->next = NULL;
    return tok;
}


