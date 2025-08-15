/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 19:34:20 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/15 20:07:30 by djuarez          ###   ########.fr       */
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

void	handle_redirections_and_quotes(t_redir *redirs, char **envp)
{
	t_redir	*redir;

	redir = redirs;
	while (redir)
	{
		redir->file = remove_quotes(redir->file);
		redir = redir->next;
	}
	handle_redirections(redirs, envp);
}

void	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	exec_path = find_executable(cmd->argv[0]);
	if (!exec_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	execute_execve(exec_path, cmd->argv, envp);
}

/*char	*str_append(char *dest, const char *src)
{
	char	*new_str;
	printf("str_append: dest=%p src=%p\n", dest, src);
	if (!src)
		return (dest);
	if (!dest)
		return (ft_strdup(src));
	new_str = ft_strjoin(dest, src);
	if (!new_str)
		return (NULL);
	printf("freeing dest=%p\n", dest);
	free (dest);
	return (new_str);
}*/
