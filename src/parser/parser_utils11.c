/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils11.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 22:11:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/19 16:16:26 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	has_expandable_chars(t_fragment *frag)
{
	int	i;

	i = 0;
	while (frag->text[i])
	{
		if (frag->text[i] == '$')
			return (1);
		if (frag->text[i] == '~' && i == 0 && frag->quote_type == QUOTE_NONE)
			return (1);
		i++;
	}
	return (0);
}

int	should_expand_fragment(t_fragment *frag)
{
	if (!frag || !frag->text)
		return (0);
	if (frag->quote_type == QUOTE_SINGLE)
		return (0);
	if (frag->quote_type == QUOTE_DOUBLE || frag->quote_type == QUOTE_NONE)
		return (has_expandable_chars(frag));
	if (frag->quote_type == QUOTE_DOLLAR)
		return (1);
	return (0);
}

/*char	*expand_fragment(const char *text, t_quote_type quote,
			char **envp, t_exec_state *state)
{
	char	*cmd;
	char	*res;
	char	*final;

	if (!text)
		return (ft_strdup(""));
	if (quote == QUOTE_SINGLE)
		return (ft_strdup(text));
	if (quote == QUOTE_DOLLAR)
		return (expand_ansi_c_string(text));
	if (contains_command_substitution(text))
	{
		cmd = extract_command_substitution(text);
		if (!cmd)
			return (ft_strdup(""));
		res = execute_command_substitution(cmd, envp);
		free(cmd);
		if (!res)
			res = ft_strdup("");
		final = expand_variables(res, envp, state, QUOTE_NONE);
		free(res);
		return (final);
	}
	if (text[0] == '~' && quote == QUOTE_NONE)
		return (expand_tilde_bash(text, envp));
	return (expand_variables(text, envp, state, quote));
}*/

char	*expand_fragment(const char *text, t_quote_type quote,
			char **envp, t_exec_state *state)
{
	char		*result;
	char		*cmd;
	char		*cmd_out;
	char		*before;
	char		*after;
	char		*tmp;
	const char	*start;
	const char	*end;
	int			len;
	int			paren_count;
	int			out_len;

	if (!text)
		return (ft_strdup(""));
	if (quote == QUOTE_SINGLE)
		return (ft_strdup(text));
	if (quote == QUOTE_DOLLAR)
		return (expand_ansi_c_string(text));
	if (text[0] == '~' && quote == QUOTE_NONE)
		return (expand_tilde_bash(text, envp));
	result = ft_strdup(text);
	if (!result)
		return (NULL);
	start = strstr(result, "$(");
	while (start)
	{
		start += 2;
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
			break ;
		len = end - start - 1;
		cmd = malloc(len + 1);
		if (!cmd)
			break ;
		strncpy(cmd, start, len);
		cmd[len] = '\0';
		cmd_out = execute_command_substitution(cmd, envp);
		free(cmd);
		if (!cmd_out)
			cmd_out = ft_strdup("");
		out_len = ft_strlen(cmd_out);
		if (out_len > 0 && cmd_out[out_len - 1] == '\n')
			cmd_out[out_len - 1] = '\0';
		before = ft_substr(result, 0, start - result - 2);
		after = ft_strdup(end);
		free(result);
		tmp = ft_strjoin(before, cmd_out);
		result = ft_strjoin(tmp, after);
		free(before);
		free(after);
		free(tmp);
		free(cmd_out);
		start = strstr(result, "$(");
	}
	tmp = expand_variables(result, envp, state, quote);
	free(result);
	return (tmp);
}

void	expand_fragments(t_token *tok, char **envp, t_exec_state *state)
{
	t_fragment	*frag;

	frag = tok->fragments;
	while (frag)
	{
		if (should_expand_fragment(frag))
			frag->expanded_text = expand_fragment(frag->text,
					frag->quote_type, envp, state);
		else
			frag->expanded_text = ft_strdup(frag->text);
		frag = frag->next;
	}
}
