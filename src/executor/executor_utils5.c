/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils5.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:56:44 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 21:22:52 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_executable(char *cmd, char **envp)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = env_get_value(envp, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin_free(full_path, cmd);
		if (access(full_path, X_OK) == 0)
		{
			return (free_split(paths), full_path);
		}
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

int	execute_execve(char *exec_path, char **argv, char **envp)
{
	int		idx;
	char	*new_var;
	int		code;

	idx = env_find_index(envp, "_");
	if (idx >= 0)
	{
		free(envp[idx]);
		new_var = ft_strjoin("_=", exec_path);
		if (new_var)
			envp[idx] = new_var;
		else
			envp[idx] = ft_strdup("_=");
	}
	if (execve(exec_path, argv, envp) == -1)
	{
		if (errno == ENOENT)
		{
			fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
			code = 127;
		}
		else if (errno == EACCES || errno == EPERM
			|| errno == EISDIR || errno == ENOEXEC)
		{
			if (errno == EISDIR)
				fprintf(stderr, "minishell: %s: Is a directory\n", exec_path);
			else
				fprintf(stderr, "minishell: %s: Permission denied\n",
					exec_path);
			code = 126;
		}
		else
		{
			fprintf(stderr, "minishell: %s: %s\n", exec_path, strerror(errno));
			code = 1;
		}
		_exit(code);
	}
	return (0);
}

static int process_heredoc(t_redir *redir)
{
	t_fragment	*frag;
	bool		is_quoted;
	char		*built_delim;

	frag = redir->fragments;
	is_quoted = false;
	while (frag)
	{
		if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
		{
			is_quoted = true;
			break ;
		}
		frag = frag->next;
	}
	redir->quoted = is_quoted;
	built_delim = build_heredoc_delimiter(redir->file);
	if (!built_delim)
		return (1);
	free(redir->file);
	redir->file = built_delim;
	return (0);
}

static void process_remove_quotes(t_redir *redir)
{
	char *tmp;

	tmp = remove_quotes(redir->file);
	free(redir->file);
	redir->file = tmp;
}

int handle_redirections_and_quotes(t_redir *redirs, char **envp, t_exec_state *state)
{
	t_redir	*redir;
	int		res;

	redir = redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			res = process_heredoc(redir);
			if (res != 0)
				return (1);
		}
		else
			process_remove_quotes(redir);
		redir = redir->next;
	}
	res = handle_redirections(redirs, envp, state);
	return (res);
}
