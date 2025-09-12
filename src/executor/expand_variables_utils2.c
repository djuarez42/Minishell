/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variables_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 20:06:50 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/12 21:17:05 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

char	*create_backslash_string(int count)
{
	char	*result;
	int		i;

	if (count <= 0)
		return (ft_strdup(""));
	result = malloc(count + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (i < count)
	{
		result[i] = '\\';
		i++;
	}
	result[count] = '\0';
	return (result);
}

void	handle_backslash_dollar_parity(const char *input, int *i, 
											int *backslashes_out, bool *should_expand)
{
	int	backslash_count;

	backslash_count = 0;
	// Count consecutive backslashes
	while (input[*i] == '\\')
	{
		backslash_count++;
		(*i)++;
	}
	// Check if we have a dollar sign after backslashes
	if (input[*i] == '$')
	{
		if (backslash_count % 2 == 0)
		{
			// Even number of backslashes: output half, then expand variable
			*backslashes_out = backslash_count / 2;
			*should_expand = true;
		}
		else
		{
			// Odd number of backslashes: output half, then literal $
			*backslashes_out = backslash_count / 2;
			*should_expand = false;
			// Move back to the $ so it gets processed as a literal
			(*i)--; 
		}
		// Skip the dollar sign only if we're expanding the variable
		if (*should_expand)
			(*i)++;
	}
	else
	{
		// No dollar sign after backslashes, treat as literal backslashes
		*backslashes_out = backslash_count;
		*should_expand = false;
	}
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
	
	// $" and $' cases: $ should disappear, leaving quotes to be handled by tokenizer
	start = *i + 1;
	if (input[start] == '"' || input[start] == '\'')
	{
		*i = *i + 1; // Skip just the $
		return (ft_strdup("")); // $ disappears completely
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
	int		backslashes_out;
	bool	should_expand;

	if (!input || !i)
		return (NULL);
	start = *i;
	// Look for backslash sequences or dollar signs
	while (input[*i] && input[*i] != '$')
	{
		if (input[*i] == '\\')
		{
			// Check if this is a backslash-dollar sequence
			int temp_i = *i;
			handle_backslash_dollar_parity(input, &temp_i, &backslashes_out, &should_expand);
			if (should_expand || backslashes_out > 0)
			{
				// We found a backslash-dollar sequence, stop here
				break;
			}
			// Not a backslash-dollar sequence, advance past the backslash
			(*i)++;
		}
		else
		{
			(*i)++;
		}
	}
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
			free(redir->file);
			redir->file = expanded;
		}
		redir = redir->next;
	}
	return (0);
}
