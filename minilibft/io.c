#include "mini_libft.h"
#include <unistd.h>

void ft_putstr_fd(const char *s, int fd)
{
    if (s == NULL)
        return;
    write(fd, s, ft_strlen(s));
}

void ft_putchar_fd(char c, int fd)
{
    write(fd, &c, 1);
}

void ft_putendl_fd(const char *s, int fd)
{
    ft_putstr_fd(s, fd);
    ft_putchar_fd('\n', fd);
}
