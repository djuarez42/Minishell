/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42barcelona.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 20:15:57 by djuarez           #+#    #+#             */
/*   Updated: 2024/07/09 20:41:14 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>
#include <stdio.h>

static int	count_words(char const *s, char c)

{
	int	count;
	int	i;

	count = 0;
	i = 0;
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
/*
int main(void)
{
    char const      *str;
    char        c;
    int     count;


    str = "hola me llamo cristhian";
    c = ' ';
    count = count_words(str, c);
    printf("la cantidad de palablas \"%s\" es de : %d\n", str, count);
    return(0);
}
*/

static char	*get_next_word(char const *s, char c, int *start)
{
	int		i;
	char	*word;

	i = *start;
	while (s[i] != '\0' && s[i] == c)
		i++;
	*start = i;
	while (s[i] != '\0' && s[i] != c)
		i++;
	word = ft_substr(s, *start, i - *start);
	*start = i;
	return (word);
}

/*int main(void)
{
    int     start = 0;
    char                *word;

    while ((word = get_next_word(s, ',', &start)) != NULL)
    {
        printf("palabra: %s\n", word);
        free(word);
        return (0);
    }
}*/
static	void	free_result(char **result)
{
	int	i;

	i = 0;
	while (result[i] != NULL)
	{
		free(result[i]);
		i++;
	}
	free(result);
}

char	**ft_split(char const *s, char c)

{
	char		**result;
	int			word_count;
	int			i;
	int			start;

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
/*int main(void)
{
    char const *str = "hola me llamo cristhian";
    char    c = ' ';
    char    **result;
    int     i;

    result = ft_split(str, c);
    if (result != NULL)
    {
        i = 0;
        while (result[i] != NULL)
        {
            printf("palabra %d: %s\n", i, result[i]);
            free(result[i]);
            i++;
        }
        free(result);
    }
    return(0);
}
*/
