/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/27 20:15:59 by djuarez          ###   ########.fr       */
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
	while (input[*i] && input [*i] != '$')
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
	size_t			j;
	char			*expanded;
	t_quote_type	quote_type;

	j = 0;
	while (argv && argv[j])
	{
		if (argv_quote && argv_quote[j] == QUOTE_SINGLE)
		{
			j++;
			continue ;
		}

		if (argv_quote)
			quote_type = argv_quote[j];
		else
			quote_type = QUOTE_NONE;

		expanded = expand_variables(argv[j], envp, state, quote_type);
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
	char			*expanded;
	t_quote_type	quote_type;

	while (redir)
	{
		if (redir->file)
		{
			if (redir->quoted)
				quote_type = QUOTE_SINGLE; // o QUOTE_DOUBLE según cómo detectes quotes en redir
			else
				quote_type = QUOTE_NONE;

			expanded = expand_variables(redir->file, envp, state, quote_type);
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
