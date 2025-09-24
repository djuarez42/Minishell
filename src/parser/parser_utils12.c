/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils12.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 16:39:47 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/24 20:56:29 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*static const char	*find_matching_paren(const char *start)
{
	int			paren_count;
	const char	*end;

	end = start;
	paren_count = 1;
	while (*end && paren_count)
	{
		if (*end == '(')
			paren_count++;
		else if (*end == ')')
			paren_count--;
		end++;
	}
	if (paren_count != 0)
		return (NULL);
	return (end);
}

static char	*extract_subcommand(const char *start, const char *end)
{
	int		len;
	char	*cmd;

	len = end - start - 1;
	cmd = malloc(len + 1);
	if (!cmd)
		return (NULL);
	ft_strlcpy(cmd, start, len + 1);
	cmd[len] = '\0';
	return (cmd);
}

static char	*run_subcommand(char *cmd, char **envp)
{
	char	*cmd_out;
	int		len;

	cmd_out = execute_command_substitution(cmd, envp);
	free(cmd);
	if (!cmd_out)
		return (ft_strdup(""));
	len = ft_strlen(cmd_out);
	if (len > 0 && cmd_out[len - 1] == '\n')
		cmd_out[len - 1] = '\0';
	return (cmd_out);
}

static char	*rebuild_result(char *result,
				const char *start, const char *end, char *cmd_out)
{
	char	*before;
	char	*after;
	char	*tmp;
	char	*new_res;

	before = ft_substr(result, 0, start - result - 2);
	after = ft_strdup(end);
	free(result);
	tmp = ft_strjoin(before, cmd_out);
	new_res = ft_strjoin(tmp, after);
	free(before);
	free(after);
	free(tmp);
	free(cmd_out);
	return (new_res);
}

char	*expand_command_substitutions(const char *text, char **envp)
{
	char		*result;
	const char	*start;
	const char	*end;
	char		*cmd;
	char		*cmd_out;

	result = ft_strdup(text);
	if (!result)
		return (NULL);
	start = ft_strstr(result, "$(");
	while (start)
	{
		start += 2;
		end = find_matching_paren(start);
		if (!end)
			break ;
		cmd = extract_subcommand(start, end);
		if (!cmd)
			break ;
		cmd_out = run_subcommand(cmd, envp);
		result = rebuild_result(result, start, end, cmd_out);
		start = ft_strstr(result, "$(");
	}
	return (result);
}*/

#include "minishell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Encuentra el paréntesis de cierre correspondiente */
static const char *find_matching_paren(const char *start)
{
    int paren_count = 1; // ya tenemos '('
    const char *ptr = start; // apunta al '('

    printf("DEBUG: find_matching_paren start at '%s'\n", ptr);

    while (*ptr && paren_count > 0)
    {
        printf("DEBUG: char='%c' paren_count=%d\n", *ptr, paren_count);
        if (*ptr == '(')
            paren_count++;
        else if (*ptr == ')')
            paren_count--;

        ptr++;
    }

    if (paren_count != 0)
    {
        printf("DEBUG: unmatched parentheses!\n");
        return NULL;
    }

    printf("DEBUG: matching paren found at '%c'\n", *(ptr - 1));
    return ptr - 1;
}


/* Extrae el subcomando entre '$(' y ')' */
static char *extract_subcommand(const char *start, const char *end)
{
    int len = end - start;
    char *cmd = malloc(len + 1);
    if (!cmd)
        return NULL;

    ft_strlcpy(cmd, start, len + 1);
    cmd[len] = '\0';

    printf("DEBUG: extracted subcommand: '%s'\n", cmd);
    return cmd;
}

/* Ejecuta el subcomando y devuelve su salida */
static char *run_subcommand(char *cmd, char **envp)
{
    char *cmd_out = execute_command_substitution(cmd, envp);
    free(cmd);

    if (!cmd_out)
        return ft_strdup("");

    int len = ft_strlen(cmd_out);
    if (len > 0 && cmd_out[len - 1] == '\n')
        cmd_out[len - 1] = '\0'; // eliminar salto de línea final

    printf("DEBUG: subcommand output: '%s'\n", cmd_out);
    return cmd_out;
}

/* Reconstruye el string reemplazando $(...) por la salida */
static char *rebuild_result(char *result,
                            const char *start, const char *end, char *cmd_out)
{
    char *before = ft_substr(result, 0, start - result); // antes de '$('
    char *after = ft_strdup(end + 1);                    // después del ')'
    char *tmp = ft_strjoin(before, cmd_out);
    char *new_res = ft_strjoin(tmp, after);

    free(before);
    free(after);
    free(tmp);
    free(cmd_out);
    free(result);

    printf("DEBUG: rebuild_result -> '%s'\n", new_res);
    return new_res;
}

/* Expande todas las sustituciones de comando en el string */
char *expand_command_substitutions(const char *text, char **envp)
{
    char *result = ft_strdup(text);
    const char *start;
    const char *end;
    char *cmd;
    char *cmd_out;

    if (!result)
        return NULL;

    start = ft_strstr(result, "$(");
    while (start)
    {
        printf("DEBUG: found '$(' at position %ld\n", start - result);

        end = find_matching_paren(start + 1); // pasar puntero al '('
        if (!end)
            break;

        cmd = extract_subcommand(start + 2, end); // extraer subcomando
        if (!cmd)
            break;

        cmd_out = run_subcommand(cmd, envp);
        result = rebuild_result(result, start, end, cmd_out);

        start = ft_strstr(result, "$("); // buscar siguiente
    }

    printf("DEBUG: final expanded result='%s'\n", result);
    return result;
}
