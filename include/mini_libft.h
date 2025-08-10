#ifndef MINI_LIBFT_H
#define MINI_LIBFT_H

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

int     ft_isspace(int c);
size_t  ft_strlen(const char *s);
size_t  ft_strlcat(char *dst, const char *src, size_t dstsize);
int     ft_strncmp(const char *s1, const char *s2, size_t n);
char    *ft_strdup(const char *s1);
char    *ft_substr(char const *s, unsigned int start, size_t len);
void    ft_putstr_fd(const char *s, int fd);
void    ft_putchar_fd(char c, int fd);
void    ft_putendl_fd(const char *s, int fd);
char    *ft_strchr(const char *s, int c);
int     ft_atoi(const char *str);
int     ft_isalpha(int c);
int     ft_isalnum(int c);
char    **ft_split(const char *s, char c);
char    *ft_strjoin(const char *s1, const char *s2);
char    *ft_strjoin_free(char *s1, const char *s2);

#endif
