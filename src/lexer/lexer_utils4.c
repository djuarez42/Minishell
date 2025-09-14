/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/30 20:11:26 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/15 01:38:17 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int check_unmatched_quotes(const char *input)
{
    int single_open = 0;
    int double_open = 0;
    int i = 0;

    while (input[i])
    {
        if (input[i] == '\'' && double_open == 0) // solo cuenta si NO estás dentro de ""
            single_open = !single_open;
        else if (input[i] == '"' && single_open == 0) // solo cuenta si NO estás dentro de ''
            double_open = !double_open;
        i++;
    }
    if (single_open || double_open)
    {
        fprintf(stderr, "minishell: syntax error: unmatched quotes\n");
        return (1); // error
    }
    return (0); // ok
}
