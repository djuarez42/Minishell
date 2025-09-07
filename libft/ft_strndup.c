/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/07 20:15:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/07 20:30:00 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

/*
 * Duplicate at most n characters from the string s
 * Always adds a terminating null byte
 */
char *ft_strndup(const char *s, size_t n)
{
    size_t	len;
    char	*dup;

    if (!s)
        return (NULL);
        
    len = ft_strlen(s);
    if (n < len)
        len = n;
        
    dup = (char *)malloc(len + 1);
    if (!dup)
        return (NULL);
        
    ft_memcpy(dup, s, len);
    dup[len] = '\0';
    
    return (dup);
}
