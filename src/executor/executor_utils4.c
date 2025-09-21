/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:52:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 19:14:34 by djuarez          ###   ########.fr       */
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


void	free_split(char **split)
{
	int	i;

	i = 0;
	while (split && split[i])
		free(split[i++]);
	free(split);
}

int handle_redirections_and_quotes(t_redir *redirs, char **envp, t_exec_state *state)
{
	t_redir		*redir;
	t_fragment	*frag;
	bool		is_quoted;
	char		*built_delim;
	char		*tmp;
	int			res;

	redir = redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			frag = redir->fragments;
			is_quoted = false;
			while (frag)
			{
				if (frag->quote_type == QUOTE_SINGLE
					|| frag->quote_type == QUOTE_DOUBLE)
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
		}
		else
		{
			tmp = remove_quotes(redir->file);
			free(redir->file);
			redir->file = tmp;
		}
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

char *build_heredoc_delimiter(const char *text)
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
	free_fragments(fragments);
	return (delimiter);
}
