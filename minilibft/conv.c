#include "mini_libft.h"

int ft_atoi(const char *nptr)
{
    size_t i;
    int mult;
    int res;

    i = 0;
    res = 0;
    mult = 1;
    while (nptr[i] == 32 || (nptr[i] >= 9 && nptr[i] <= 13))
        i++;
    if (nptr[i] == '-' || nptr[i] == '+')
    {
        if (nptr[i] == '-')
            mult = -1;
        i++;
    }
    while (nptr[i] >= 48 && nptr[i] <= 57)
    {
        res = res * 10 + (nptr[i] - 48);
        i++;
    }
    return (res * mult);
}

int ft_isalpha(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return (1);
    return (0);
}

int ft_isalnum(int c)
{
    if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122))
        return (1);
    return (0);
}
