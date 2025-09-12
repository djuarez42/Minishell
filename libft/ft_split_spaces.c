/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_spaces.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 19:02:30 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/11 19:09:20 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

char **ft_split_spaces(const char *s)
{
    int i = 0, word_count = 0;
    char **res;
    int start, w = 0;

    if (!s)
        return NULL;
    while (s[i])
    {
        while (s[i] && ft_isspace((unsigned char)s[i]))
            i++;
        if (!s[i])
            break;
        word_count++;
        while (s[i] && !ft_isspace((unsigned char)s[i]))
            i++;
    }
    res = malloc(sizeof(char *) * (word_count + 1));
    if (!res)
        return NULL;
    i = 0;
    while (*s)
    {
        while (*s && ft_isspace((unsigned char)*s))
            s++;
        if (!*s)
            break;
        start = 0;
        while (s[start] && !ft_isspace((unsigned char)s[start]))
            start++;
        res[w++] = ft_strndup(s, start);
        s += start;
    }
    res[w] = NULL;
    return res;
}