/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:52:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/27 19:27:52 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_split(char **split)
{
	int	i;

	i = 0;
	while (split && split[i])
		free(split[i++]);
	free(split);
}

int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		write(2, "minishell: syntax error near unexpected token `|'\n", 48);
		return (2);
	}
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		if (ft_strchr(cmd->argv[0], '/'))
		{
			ft_putstr_fd("minishell: ", 2);
			if (!isatty(STDIN_FILENO))
				ft_putstr_fd("line 1: ", 2);
			ft_putstr_fd((char *)cmd->argv[0], 2);
			ft_putstr_fd(": No such file or directory\n", 2);
		}
		else
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd((char *)cmd->argv[0], 2);
			ft_putstr_fd(": command not found\n", 2);
		}
		return (127);
	}
	return (execute_execve(exec_path, cmd->argv, envp));
}

char	*str_append(char *base, const char *add)
{
	char	*new;
	size_t	len;

	len = 0;
	if (base)
		len += ft_strlen(base);
	if (add)
		len += ft_strlen(add);
	new = malloc(len + 1);
	if (!new)
		return (NULL);
	new[0] = '\0';
	if (base)
		ft_strlcat(new, base, len + 1);
	if (add)
		ft_strlcat(new, add, len + 1);
	free(base);
	return (new);
}

char	*build_heredoc_delimiter(const char *text)
{
	t_fragment	*fragments;
	char		*delimiter;
	t_token		dummy_tok;

	delimiter = NULL;
	if (!text)
		return (ft_strdup(""));
	fragments = parse_mixed_fragments(text);
	if (!fragments)
		return (ft_strdup(text));
	dummy_tok.fragments = fragments;
	delimiter = concat_final_text(&dummy_tok);
	free_fragments(fragments);
	return (delimiter);
}
