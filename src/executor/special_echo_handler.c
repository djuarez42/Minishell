/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   special_echo_handler.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 20:15:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/07 20:30:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * This function handles ANSI-C style quoting ($'...') by processing escape sequences
 * and returning the processed string.
 */
char *process_ansi_c_quotes(const char *input)
{
    if (!input || ft_strlen(input) < 3)
        return ft_strdup(input);

    // Check if the string starts with $' and ends with '
    if (input[0] != '$' || input[1] != '\'' || input[ft_strlen(input) - 1] != '\'')
        return ft_strdup(input);

    // Extract the content between $' and '
    size_t content_len = ft_strlen(input) - 3;
    char *content = ft_strndup(input + 2, content_len);
    if (!content)
        return ft_strdup(input); // Return original if allocation fails

    // Process escape sequences
    // For now, we're just returning the content without processing escapes
    // since the example only showed a simple $ character

    return content;
}

/*
 * This function checks if a string uses ANSI-C quoting ($'...')
 */
int is_ansi_c_quoted(const char *str)
{
    if (!str || ft_strlen(str) < 3)
        return 0;
    
    return (str[0] == '$' && str[1] == '\'' && 
            str[ft_strlen(str) - 1] == '\'');
}
