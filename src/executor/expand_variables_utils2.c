/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/15 16:36:06 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*handle_special_dollar(const char *input, int *i, t_exec_state *state)
{
	int	start;

	start = *i + 1;
	printf("DEBUG: handle_special_dollar called at index %d, char='%c'\n", *i, input[start]); // <- al inicio
	if (!input[start])
	{
		*i = *i + 1;
		printf("DEBUG: special dollar at end of string, returning '$'\n"); // <- antes de return
		return (ft_strdup("$"));
	}
	if (input[start] == '?')
	{
		*i = start + 1;
		printf("DEBUG: special dollar for $? , returning exit status\n"); // <- antes de return
		return (expand_exit_status(state));
	}
	printf("DEBUG: not a special dollar, returning NULL\n"); // <- antes de return NULL
	return (NULL);
}

char	*handle_dollar(const char *input, int *i, char **envp,
	t_exec_state *state)
{
	int		start;
	int		len;
	char	*res;
	char	*name;

	if (!input || !i)
		return (NULL);
	printf("DEBUG: handle_dollar called at index %d, char='%c'\n", *i, input[*i + 1]); // <- al inicio
	res = handle_special_dollar(input, i, state);
	if (res)
	{
		printf("DEBUG: handle_special_dollar returned a value: '%s'\n", res); // <- si retorna algo
		return (res);
	}
	start = *i + 1;
	len = skip_variable_name(input + start);
	if (len == 0)
	{
		*i = *i + 1;
		printf("DEBUG: no variable name after $, returning '$'\n"); // <- antes de return
		return (ft_strdup("$"));
	}
	name = ft_substr(input, start, len);
	if (!name)
		return (NULL);
	res = expand_env_var(name, envp);
	free(name);
	printf("DEBUG: expanded variable '%s' -> '%s'\n", name, res); // <- antes de return
	*i = start + len;
	return (res);
}

char	*extract_plain_text(const char *input, int *i, char *tmp)
{
	int		start;
	char	*segment;

	if (!input || !i)
		return (NULL);
	start = *i;
	while (input[*i] && input [*i] != '$')
		(*i)++;
	if (start == *i)
		return (tmp);
	segment = ft_substr(input, start, *i - start);
	if (!segment)
		return (NULL);
	tmp = str_append(tmp, segment);
	free (segment);
	if (!tmp)
		return (NULL);
	return (tmp);
}

int	expand_argv(char **argv, char **envp, t_exec_state *state)
{
	size_t	j;
	char	*expanded;

	j = 0;
	while (argv && argv[j])
	{
		expanded = expand_variables(argv[j], envp, state);
		if (!expanded)
			return (-1);
		free (argv[j]);
		argv[j] = expanded;
		j++;
	}
	return (0);
}

int	expand_redirs(t_redir *redir, char **envp, t_exec_state *state)
{
	char	*expanded;

	while (redir)
	{
		if (redir->file)
		{
			expanded = expand_variables(redir->file, envp, state);
			if (!expanded)
				return (-1);
			free(redir->file);
			redir->file = expanded;
		}
		redir = redir->next;
	}
	return (0);
}
