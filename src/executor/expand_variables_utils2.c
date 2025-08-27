/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/27 20:16:49 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*handle_special_dollar(const char *input, int *i, t_exec_state *state)
{
	int	start;

	start = *i + 1;
	if (!input[start])
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	if (input[start] == '?')
	{
		*i = start + 1;
		return (expand_exit_status(state));
	}
	if (input[start] == '$')
	{
		*i = start + 1;
		return (ft_strdup("$"));
	}
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
	res = handle_special_dollar(input, i, state);
	if (res)
		return (res);
	start = *i + 1;
	len = skip_variable_name(input + start);
	if (len == 0)
	{
		*i = *i + 1;
		return (ft_strdup("$"));
	}
	name = ft_substr(input, start, len);
	if (!name)
		return (NULL);
	res = expand_env_var(name, envp);
	free(name);
	*i = start + len;
	return (res);
}

char	*extract_plain_text(const char *input, int *i, char *tmp)
{
	int		start;
	char	*segment;
	char	*new_tmp;

	if (!input || !i)
		return (NULL);
	start = *i;
	while (input[*i] && 
		input[*i] != '$' && 
		!(input[*i] == '\\' && input[*i+1] == '$'))
		(*i)++;
	if (start == *i)
		return (tmp);
	segment = ft_substr(input, start, *i - start);
	if (!segment)
	{
		free(tmp);
		return (NULL);
	}
	new_tmp = str_append(tmp, segment);
	if (!new_tmp)
	{
		free(segment);
		free(tmp);
		return (NULL);
	}
	free(segment);
	return (new_tmp);
}

int	expand_argv(char **argv, t_quote_type *argv_quote,
		char **envp, t_exec_state *state)
{
	size_t	j;
	char	*expanded;

	j = 0;
	while (argv && argv[j])
	{
		// If the string was originally in single quotes, preserve it exactly as-is
		if (argv_quote && argv_quote[j] == QUOTE_SINGLE)
		{
			// We don't need to do anything - keep the string as is with single quotes
			j++;
			continue;
		}
		
		// For other strings, including those in double quotes, expand variables
		expanded = expand_variables(argv[j], envp, state);
		if (!expanded)
			return (-1);
		free(argv[j]);
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
			{
				free(redir->file);
				return (-1);
			}
			if (redir->file != expanded)
				free(redir->file);
			redir->file = expanded;
		}
		redir = redir->next;
	}
	return (0);
}
