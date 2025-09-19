/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils12.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 16:39:47 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/19 16:46:45 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*find_matching_paren(const char *start)
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
	strncpy(cmd, start, len);
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
	start = strstr(result, "$(");
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
		start = strstr(result, "$(");
	}
	return (result);
}
