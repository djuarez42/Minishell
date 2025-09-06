#include "mini_libft.h"
#include <stdlib.h>

// ft_strlen, ft_strlcat, ft_strncmp, ft_strdup, ft_substr

size_t  ft_strlen(const char *str)
{
    size_t i = 0;
    while (str[i] != '\0')
        i++;
    return (i);
}

size_t  ft_strlcat(char *dst, const char *src, size_t size)
{
    size_t i, lendst, lensrc, lentotal;
    lendst = 0;
    lensrc = 0;
    i = 0;
    while (dst[lendst] != '\0' && lendst < size)
        lendst++;
    while (src[lensrc])
        lensrc++;
    lentotal = lendst + lensrc;
    if (lendst >= size)
        return (size + lensrc);
    while (src[i] != '\0' && (i < (size - lendst - 1)))
    {
        dst[lendst + i] = src[i];
        i++;
    }
    dst[lendst + i] = '\0';
    return (lentotal);
}

int     ft_strncmp(const char *s1, const char *s2, size_t n)
{
    size_t i = 0;
    while (i < n && (s1[i] || s2[i]))
    {
        if ((unsigned char)s1[i] != (unsigned char)s2[i])
            return ((unsigned char)s1[i] - (unsigned char)s2[i]);
        i++;
    }
    return (0);
}

char    *ft_strdup(const char *s1)
{
    size_t len = ft_strlen(s1);
    char *dup = (char *)malloc(len + 1);
    if (!dup)
        return NULL;
    for (size_t i = 0; i < len; i++)
        dup[i] = s1[i];
    dup[len] = '\0';
    return dup;
}

char    *ft_substr(char const *s, unsigned int start, size_t len)
{
    size_t slen = ft_strlen(s);
    if (start >= slen)
        return ft_strdup("");
    if (len > slen - start)
        len = slen - start;
    char *sub = (char *)malloc(len + 1);
    if (!sub)
        return NULL;
    for (size_t i = 0; i < len; i++)
        sub[i] = s[start + i];
    sub[len] = '\0';
    return sub;
}
