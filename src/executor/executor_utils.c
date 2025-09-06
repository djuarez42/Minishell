/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:34:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/06 16:41:53 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "executor.h"
#include "minishell.h"

void	free_split(char **split)
{
	int	i;

	i = 0;
	while (split && split[i])
		free(split[i++]);
	free(split);
}

int	handle_redirections_and_quotes(t_redir *redirs, char **envp, t_exec_state *state)
{
	t_redir	*redir;
	int		res;

	redir = redirs;
	while (redir)
	{
		redir->file = remove_quotes(redir->file);
		redir = redir->next;
	}
	res = handle_redirections(redirs, envp, state);
	return (res);
}

int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		return (2);
	}
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
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