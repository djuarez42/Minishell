#include "mini_libft.h"
#include <stdlib.h>

char *ft_strchr(const char *s, int c)
{
    int i = 0;
    char *ptr = (char *)s;
    while (ptr[i] != '\0' && (ptr[i] != (unsigned char)c))
        i++;
    if (ptr[i] == (unsigned char)c)
        return (&ptr[i]);
    return (NULL);
}

char *ft_strjoin(const char *s1, const char *s2)
{
    int len1, len2;
    char *str;
    if (s1 == NULL || s2 == NULL)
        return (NULL);
    len1 = ft_strlen(s1);
    len2 = ft_strlen(s2);
    str = (char *)malloc((len1 + len2 + 1) * sizeof(char));
    if (!str)
        return (NULL);
    ft_strlcpy(str, s1, len1 + 1);
    ft_strlcat(str, s2, len1 + len2 + 1);
    return (str);
}

char *ft_strjoin_free(char *s1, const char *s2)
{
    char *joined;
    if (!s1)
    {
        if (!s2)
            return (NULL);
        return (ft_strdup(s2));
    }
    if (!s2)
        return (NULL);
    joined = ft_strjoin(s1, s2);
    if (!joined)
    {
        free(s1);
        return (NULL);
    }
    free(s1);
    return (joined);
}

static int count_words(char const *s, char c)
{
    int count = 0, i = 0;
    while (s[i] != '\0')
    {
        while (s[i] == c && s[i] != '\0')
            i++;
        if (s[i] != '\0')
        {
            count++;
            while (s[i] != c && s[i] != '\0')
                i++;
        }
    }
    return (count);
}

static char *get_next_word(char const *s, char c, int *start)
{
    int i = *start;
    while (s[i] != '\0' && s[i] == c)
        i++;
    *start = i;
    while (s[i] != '\0' && s[i] != c)
        i++;
    char *word = ft_substr(s, *start, i - *start);
    *start = i;
    return word;
}

static void free_result(char **result)
{
    int i = 0;
    while (result[i] != NULL)
    {
        free(result[i]);
        i++;
    }
    free(result);
}

char **ft_split(const char *s, char c)
{
    char **result;
    int word_count, i, start;
    if (s == NULL)
        return (NULL);
    word_count = count_words(s, c);
    result = (char **)malloc((word_count + 1) * sizeof(char *));
    if (result == NULL)
        return (NULL);
    i = 0;
    start = 0;
    while (i < word_count)
    {
        result[i] = get_next_word(s, c, &start);
        if (result[i] == NULL)
        {
            free_result(result);
            return (NULL);
        }
        i++;
    }
    result[i] = NULL;
    return (result);
}

int ft_isspace(int c)
{
    return (c == ' ' || (c >= 9 && c <= 13));
}
