/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/03 21:09:54 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to create a string from a single character
static char *ft_strdupc(char c)
{
    char *str;
    
    str = (char *)malloc(sizeof(char) * 2);
    if (!str)
        return (NULL);
    str[0] = c;
    str[1] = '\0';
    return (str);
}

// Helper function to specifically handle $"string" pattern
static char *handle_dollar_string(const char *input, int *i)
{
    char *result;
    int start, end;

    if (!input)
        return ft_strdup("");

    (*i)++; // saltamos el $

    if (input[*i] == '"')
        (*i)++; // saltamos la comilla inicial

    start = *i;
    end = start;

    while (input[end] && input[end] != '"')
        end++;

    result = ft_substr(input, start, end - start);
    if (!result)
        return NULL;

    if (input[end] == '"')
        *i = end + 1; // saltamos la comilla final
    else
        *i = end;

    return result;
}

// Helper function to check for $"string" pattern
static int is_dollar_string(const char *input, int pos)
{
    if (!input || !input[pos])
        return 0;
    
    // $ seguido de "
    return (input[pos] == '$' && input[pos + 1] && input[pos + 1] == '"');
}

char *expand_variables(const char *input, char **envp, t_exec_state *state,
                       t_quote_type quote)
{
    int i = 0;
    char *tmp = NULL;
    char *piece;

    if (!input)
        return NULL;
    if (input[0] == '\0')
        return ft_strdup("");

    while (input[i])
    {
        if (input[i] == '\\' && input[i + 1] == '$')
        {
            piece = ft_strdup("$");
            tmp = str_append(tmp, piece);
            free(piece);
            i += 2;
        }
        else if (input[i] == '$')
        {
            if (is_dollar_string(input, i))
                piece = handle_dollar_string(input, &i); // devuelve literal sin expandir
            else if (quote == QUOTE_DOUBLE || quote == QUOTE_NONE)
                piece = handle_dollar_quotes_fix(input, &i, envp, state);
            else // quote simple
            {
                piece = ft_strdup("$");
                i++;
            }

            tmp = str_append(tmp, piece);
            free(piece);
        }
        else
        {
            piece = ft_strdupc(input[i]);
            tmp = str_append(tmp, piece);
            free(piece);
            i++;
        }
    }
    return tmp;
}

