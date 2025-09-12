/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:17:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/12 21:17:00 by ekakhmad         ###   ########.fr       */
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

/*
 * Helper function to try to append a string safely.
 * Takes ownership of both base and piece, and guarantees
 * they are properly freed on error or success.
 */
static char *try_append(char *base, char *piece)
{
    char *result;
    
    if (!piece)
    {
        // If piece is NULL, something went wrong - free base and return NULL
        free(base);
        return NULL;
    }
    
    // If base is NULL but piece isn't, just return piece (don't free it)
    if (!base)
        return piece;
        
    // Append the string
    result = str_append(base, piece);
    
    // Free the piece since it's been copied and str_append makes a new copy
    free(piece);
    
    // Return the result (str_append already freed base)
    return result;
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
int is_dollar_string(const char *input, int pos)
{
    if (!input || !input[pos])
        return 0;
    return (input[pos] == '$' && input[pos + 1] &&
            (input[pos + 1] == '"' || input[pos + 1] == '\''));
}

char *expand_variables(const char *input, char **envp, t_exec_state *state,
                       t_quote_type quote)
{
    int     i;
    char    *tmp;
    char    *piece;

    if (!input)
        return (NULL);
    if (input[0] == '\0')
        return (ft_strdup(""));

    i = 0;
    tmp = ft_strdup("");
    if (!tmp)
        return (NULL);
        
    while (input[i])
    {
        /* Caso especial: \$ → conservar la barra */
        if (input[i] == '\\' && input[i + 1] == '$')
        {
            piece = ft_strdup("\\$");
            if (!piece)
            {
                free(tmp);
                return NULL;
            }
            tmp = str_append(tmp, piece);
            free(piece);
            i += 2;
        }
        /* Variables normales */
        else if (input[i] == '$')
        {
            if (is_dollar_string(input, i))
                piece = handle_dollar_string(input, &i);
            else if (is_ansi_c_quote(input, i) && quote != QUOTE_SINGLE)
            {
                // Handle ANSI-C quoting - properly implement $'...' to output literal string
                // Skip $ and '
                i += 2;
                
                // Find closing quote
                int start = i;
                while (input[i] && input[i] != '\'')
                    i++;
                
                // Extract content between quotes
                char *content = ft_substr(input, start, i - start);
                if (!content)
                    return NULL;
                
                // Skip closing quote if found
                if (input[i] == '\'')
                    i++;
                    
                // Add the extracted content (without $' and ')
                tmp = try_append(tmp, content);
                if (!tmp)
                    return NULL;
                
                continue;
            }
            else if (quote == QUOTE_DOUBLE || quote == QUOTE_NONE)
                piece = handle_dollar_quotes_fix(input, &i, envp, state);
            else
            {
                piece = ft_strdup("$");
                i++;
            }
            
            tmp = try_append(tmp, piece);
            if (!tmp)
                return NULL;
        }
        /* Caracteres normales */
        else
        {
            tmp = try_append(tmp, ft_strdupc(input[i]));
            if (!tmp)
                return NULL;
            i++;
        }
    }
    return tmp;
}

char *interpret_ansi_c_escapes(const char *str)
{
    int i = 0, j = 0;
    char *res;
    size_t len = ft_strlen(str);

    res = malloc(len + 1); // tamaño máximo, luego realloc si quieres
    if (!res)
        return NULL;

    while (str[i])
    {
        if (str[i] == '\\' && str[i + 1])
        {
            i++;
            if (str[i] == 'n') res[j++] = '\n';
            else if (str[i] == 't') res[j++] = '\t';
            else if (str[i] == 'r') res[j++] = '\r';
            else if (str[i] == '\\') res[j++] = '\\';
            else if (str[i] == '\'') res[j++] = '\'';
            else if (str[i] == '"') res[j++] = '"';
            else res[j++] = str[i]; // otros escapes literales
            i++;
        }
        else
            res[j++] = str[i++];
    }
    res[j] = '\0';
    return res;
}

char *expand_ansi_c_string(const char *input)
{
    char *res;
    char *tmp;

    if (!input)
        return ft_strdup("");

    res = ft_strdup(input);
    if (!res)
        return NULL;

    tmp = res;
    res = interpret_ansi_c_escapes(res);
    free(tmp);

    return res;
}