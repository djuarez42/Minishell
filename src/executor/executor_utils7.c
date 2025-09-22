/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils7.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 21:38:58 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/21 22:08:17 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*ft_strdupc(char c)
{
	char	*str;

	str = (char *)malloc(sizeof(char) * 2);
	if (!str)
		return (NULL);
	str[0] = c;
	str[1] = '\0';
	return (str);
}

static char	*handle_dollar_string(const char *input, int *i)
{
	char	*result;
	int		start;
	int		end;

	if (!input)
		return (ft_strdup(""));
	(*i)++;
	if (input[*i] == '"')
		(*i)++;
	start = *i;
	end = start;
	while (input[end] && input[end] != '"')
		end++;
	result = ft_substr(input, start, end - start);
	if (!result)
		return (NULL);
	if (input[end] == '"')
		*i = end + 1;
	else
		*i = end;
	return (result);
}

int	is_dollar_string(const char *input, int pos)
{
	if (!input || !input[pos])
		return (0);
	return (input[pos] == '$' && input[pos + 1] && (input[pos + 1] == '"'
			|| input[pos + 1] == '\''));
}

/*char *expand_variables(const char *input, char **envp, t_exec_state *state,
					   t_quote_type quote)
{
	int     i;
	char    *tmp;
	char    *piece;

	if (!input)
		return (NULL);
	if (input[0] == '\0')
		return (ft_strdup(""));
	i = 0;
	tmp = NULL;
	while (input[i])
	{
		if (input[i] == '\\' && input[i + 1] == '$')
		{
			piece = ft_strdup("\\$");
			if (!piece)
			{
				free(tmp);
				return NULL;
			}
			tmp = str_append(tmp, piece);
			free(piece);
			i += 2;
		}
		else if (input[i] == '$')
		{
			if (is_dollar_string(input, i))
				piece = handle_dollar_string(input, &i);
			else if (quote == QUOTE_DOUBLE || quote == QUOTE_NONE)
				piece = handle_dollar_quotes_fix(input, &i, envp, state);
			else
			{
				piece = ft_strdup("$");
				i++;
			}
			tmp = str_append(tmp, piece);
			free(piece);
		}
		else
		{
			piece = ft_strdupc(input[i]);
			tmp = str_append(tmp, piece);
			free(piece);
			i++;
		}
	}
	return tmp;
}*/

static char	*handle_escaped_dollar(int *i, char *tmp)
{
	char	*piece;

	piece = ft_strdup("\\$");
	if (!piece)
	{
		free(tmp);
		return (NULL);
	}
	tmp = str_append(tmp, piece);
	free(piece);
	*i += 2;
	return (tmp);
}

static char	*handle_dollar(const char *input, int *i,
		char *tmp, t_dollar_ctx *ctx)
{
	char	*piece;

	if (is_dollar_string(input, *i))
		piece = handle_dollar_string(input, i);
	else if (ctx->quote == QUOTE_DOUBLE || ctx->quote == QUOTE_NONE)
		piece = handle_dollar_quotes_fix(input, i, ctx->envp, ctx->state);
	else
	{
		piece = ft_strdup("$");
		(*i)++;
	}
	tmp = str_append(tmp, piece);
	free(piece);
	return (tmp);
}

static char	*handle_literal(const char *input, int *i, char *tmp)
{
	char	*piece;

	piece = ft_strdupc(input[*i]);
	tmp = str_append(tmp, piece);
	free(piece);
	(*i)++;
	return (tmp);
}

char	*expand_variables(const char *input, char **envp,
						 t_exec_state *state, t_quote_type quote)
{
	int				i;
	char			*tmp;
	t_dollar_ctx	ctx;

	if (!input)
		return (NULL);
	if (input[0] == '\0')
		return (ft_strdup(""));
	i = 0;
	tmp = NULL;
	ctx.envp = envp;
	ctx.state = state;
	ctx.quote = quote;
	while (input[i])
	{
		if (input[i] == '\\' && input[i + 1] == '$')
			tmp = handle_escaped_dollar(&i, tmp);
		else if (input[i] == '$')
			tmp = handle_dollar(input, &i, tmp, &ctx);
		else
			tmp = handle_literal(input, &i, tmp);
		if (!tmp)
			return (NULL);
	}
	return (tmp);
}

/*char *interpret_ansi_c_escapes(const char *str)
{
	int i = 0, j = 0;
	char *res;
	size_t len = ft_strlen(str);

	res = malloc(len + 1); // tamaño máximo, luego realloc si quieres
	if (!res)
		return NULL;

	while (str[i])
	{
		if (str[i] == '\\' && str[i + 1])
		{
			i++;
			if (str[i] == 'n') res[j++] = '\n';
			else if (str[i] == 't') res[j++] = '\t';
			else if (str[i] == 'r') res[j++] = '\r';
			else if (str[i] == '\\') res[j++] = '\\';
			else if (str[i] == '\'') res[j++] = '\'';
			else if (str[i] == '"') res[j++] = '"';
			else res[j++] = str[i]; // otros escapes literales
			i++;
		}
		else
			res[j++] = str[i++];
	}
	res[j] = '\0';
	return res;
}*/

static char	interpret_single_escape(char c)
{
	if (c == 'n')
		return ('\n');
	if (c == 't')
		return ('\t');
	if (c == 'r')
		return ('\r');
	if (c == '\\')
		return ('\\');
	if (c == '\'')
		return ('\'');
	if (c == '"')
		return ('"');
	return (c);
}

static void	handle_escape(const char *str, int *i, char *res, int *j)
{
	(*i)++;
	res[*j] = interpret_single_escape(str[*i]);
	(*j)++;
	(*i)++;
}

char	*interpret_ansi_c_escapes(const char *str)
{
	int		i;
	int		j;
	char	*res;
	size_t	len;

	i = 0;
	j = 0;
	len = ft_strlen(str);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	while (str[i])
	{
		if (str[i] == '\\' && str[i + 1])
			handle_escape(str, &i, res, &j);
		else
			res[j++] = str[i++];
	}
	res[j] = '\0';
	return (res);
}

char *expand_ansi_c_string(const char *input)
{
	char	*res;
	char	*tmp;

	if (!input)
		return (ft_strdup(""));
	res = ft_strdup(input);
	if (!res)
		return (NULL);
	tmp = res;
	res = interpret_ansi_c_escapes(res);
	free(tmp);
	return (res);
}

/*char *execute_command_substitution(const char *cmd, char **envp)
{
	int fd[2];
	pid_t pid;
	char buffer[1024];
	int n;
	char *result = NULL;
	size_t size = 0;

	if (pipe(fd) == -1)
		return NULL;

	pid = fork();
	if (pid < 0)
		return NULL;
	else if (pid == 0)
	{
		// hijo
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[1], STDERR_FILENO);
		close(fd[1]);
		// usar envp aquí
		char *argv[] = {"/bin/sh", "-c", (char *)cmd, NULL};
		execve("/bin/sh", argv, envp);
		_exit(127);
	}
	else
	{
		// padre
		close(fd[1]);
		while ((n = read(fd[0], buffer, sizeof(buffer))) > 0)
		{
			char *tmp = realloc(result, size + n + 1);
			if (!tmp)
			{
				free(result);
				result = NULL;
				break;
			}
			result = tmp;
			memcpy(result + size, buffer, n);
			size += n;
		}
		if (result)
			result[size] = '\0';
		close(fd[0]);
		waitpid(pid, NULL, 0);
	}
	return result;
}*/

static void	exec_subst_child(const char *cmd, char **envp, int fd[2])
{
	char	*argv[4];

	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	dup2(fd[1], STDERR_FILENO);
	close(fd[1]);
	argv[0] = "/bin/sh";
	argv[1] = "-c";
	argv[2] = (char *)cmd;
	argv[3] = NULL;
	execve("/bin/sh", argv, envp);
	_exit(127);
}

/*static char	*exec_subst_parent(pid_t pid, int fd[2])
{
	char	buffer[1024];
	int		n;
	char	*result;
	size_t	size;
	char	*tmp;

	result = NULL;
	size = 0;
	close(fd[1]);
	n = read(fd[0], buffer, sizeof(buffer));
	while (n > 0)
	{
		tmp = realloc(result, size + n + 1);
		if (!tmp)
		{
			free(result);
			result = NULL;
			break ;
		}
		result = tmp;
		ft_memcpy(result + size, buffer, n);
		size += n;
		n = read(fd[0], buffer, sizeof(buffer));
	}
	if (result)
		result[size] = '\0';
	close(fd[0]);
	waitpid(pid, NULL, 0);
	return (result);
}*/

static char	*append_buffer(char *result, size_t size,
							const char *buffer, int n)
{
	char	*new_buf;

	new_buf = malloc(size + n + 1);
	if (!new_buf)
	{
		free(result);
		return (NULL);
	}
	if (result)
		ft_memcpy(new_buf, result, size);
	ft_memcpy(new_buf + size, buffer, n);
	free(result);
	return (new_buf);
}

static char	*read_from_pipe(int fd)
{
	char	buffer[1024];
	int		n;
	char	*result;
	size_t	size;

	result = NULL;
	size = 0;
	n = read(fd, buffer, sizeof(buffer));
	while (n > 0)
	{
		result = append_buffer(result, size, buffer, n);
		if (!result)
			break ;
		size += n;
		n = read(fd, buffer, sizeof(buffer));
	}
	if (result)
		result[size] = '\0';
	return (result);
}

static char	*exec_subst_parent(pid_t pid, int fd[2])
{
	char	*result;

	close(fd[1]);
	result = read_from_pipe(fd[0]);
	close(fd[0]);
	waitpid(pid, NULL, 0);
	return (result);
}

char	*execute_command_substitution(const char *cmd, char **envp)
{
	int		fd[2];
	pid_t	pid;

	if (pipe(fd) == -1)
		return (NULL);
	pid = fork();
	if (pid < 0)
		return (NULL);
	else if (pid == 0)
		exec_subst_child(cmd, envp, fd);
	return (exec_subst_parent(pid, fd));
}
