/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/28 19:41:57 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_tilde_prefix(const char *s, char **envp)
{
	const char	*home;
	char		*suffix;
	char		*res;

	if (!s || s[0] != '~')
		return (ft_strdup(s));
	if (s[1] && s[1] != '/')
		return (ft_strdup(s));
	home = env_get_value(envp, "HOME");
	if (!home)
		home = "";
	suffix = ft_strdup(s + 1);
	if (!suffix)
		return (NULL);
	res = ft_strjoin(home, suffix);
	free(suffix);
	return (res);
}

char	*remove_all_quotes(const char *s)
{
	size_t	len;
	size_t	i;
	size_t	j;
	char	*out;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	while (i < len)
	{
		if (s[i] != '\'' && s[i] != '"')
			out[j++] = s[i];
		i++;
	}
	out[j] = '\0';
	return (out);
}

char	*handle_special_dollar(const char *input, int *i, t_exec_state *state)
{
	int	start;

	start = *i + 1;
	if (!input[start])
	{
		*i = *i + 1;
		return (ft_strdup(""));
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
	
	// $" and $' cases: based on bash behavior, $ does NOT disappear
	// These should be treated as invalid variable names, so return literal $
	start = *i + 1;
	if (input[start] == '"' || input[start] == '\'')
	{
		*i = *i + 1; // Skip just the $
		return (ft_strdup("$"));
	}

	// $<digit> case: expand single digit as env var name
	if (ft_isdigit((unsigned char)input[start]))
	{
		char name[2] = { input[start], '\0' };
		*i = start + 1;
		return (expand_env_var(name, envp));
	}

	// Normal variable expansion
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
	while (input[*i] && input[*i] != '$' && !(input[*i] == '\\' && input[*i+1] == '$'))
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
	t_quote_type quote_type;

	j = 0;
	while (argv && argv[j])
	{
		// For complex quotes, we need to use the quote type from argv_quote
		quote_type = argv_quote ? argv_quote[j] : QUOTE_NONE;
		
		// Skip expansion only for pure single-quoted strings
		if (quote_type == QUOTE_SINGLE)
		{
			j++;
			continue;
		}
		
		// First apply tilde expansion (for non-single-quoted tokens)
		char *tilde_expanded = expand_tilde_prefix(argv[j], envp);
		if (!tilde_expanded)
			return (-1);
		
		// Then apply variable expansion
		expanded = expand_variables(tilde_expanded, envp, state, quote_type);
		free(tilde_expanded);
		
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
			expanded = expand_variables(redir->file, envp, state, QUOTE_NONE);
			if (!expanded)
			{
				free(redir->file);
				return (-1);
			}
			char *stripped = remove_all_quotes(expanded);
			if (!stripped)
			{
				free(expanded);
				free(redir->file);
				return (-1);
			}
			free(redir->file);
			free(expanded);
			redir->file = stripped;
		}
		redir = redir->next;
	}
	return (0);
}
