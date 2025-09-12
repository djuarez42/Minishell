/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 17:34:49 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/12 21:17:11 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <string.h> // For memset

void handle_redirections_out(const char *filename, int *error)
{
    int fd;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        print_error_file(filename);
        *error = 1;
        return ;
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        print_error_file("dup2");
        close(fd);
        *error = 1;
        return ;
    }
    close(fd);
}

void handle_redirections_in(const char *filename, int *error)
{
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        print_error_file(filename);
        *error = 1;
        return ;
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        print_error_file("dup2");
        close(fd);
        *error = 1;
        return ;
    }
    close(fd);
}

void handle_redirections_append(const char *filename, int *error)
{
    int fd;

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        print_error_file(filename);
        *error = 1;
        return ;
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        print_error_file("dup2");
        close(fd);
        *error = 1;
        return ;
    }
    close(fd);
}

int	handle_redirections_heredoc(const char *delimiter, bool quoted,
			char **envp, t_heredoc_args *args)
{
	int	fd;
	int	res;

	args->delimiter = delimiter;
	args->quoted = quoted;
	args->envp = envp;
	args->heredoc_path = NULL;

	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);

	args->fd = fd;

	res = write_heredoc_lines(args);
	close(fd);

	if (res == 130)
		return (130);

	redirect_stdin_heredoc(args->heredoc_path);
	return 0;
}

int	handle_redirections_heredoc_with_content(char **heredoc_content, bool quoted,
			char **envp, t_exec_state *state, t_heredoc_args *args)
{
	int		fd;
	int		i;
	char	*expanded_line;

	fd = open_heredoc_file(args);
	if (fd == -1)
		return (1);
	
	// Write pre-collected content to temp file
	i = 0;
	while (heredoc_content[i])
	{
		printf("heredoc debug: '%s'\n", heredoc_content[i]);
		if (!quoted)
			expanded_line = expand_variables(heredoc_content[i], envp, state, QUOTE_NONE);
		else
			expanded_line = ft_strdup(heredoc_content[i]);
        
		if (!expanded_line)
		{
			close(fd);
			return (1);
		}
        
		write(fd, expanded_line, ft_strlen(expanded_line));
		write(fd, "\n", 1);
		free(expanded_line);
		i++;
	}
	
	close(fd);
	redirect_stdin_heredoc(args->heredoc_path);
	return (0);
}

int	handle_redirections(t_redir *redir, char **envp, t_exec_state *state)
{
	int			res;
	int			err;
	t_heredoc_args	args;

	err = 0;
	memset(&args, 0, sizeof(args));
	while (redir)
	{
		// Skip redirections with NULL file when necessary
		if (redir->type == TOKEN_REDIRECT_OUT)
		{
			if (redir->file)
				handle_redirections_out(redir->file, &err);
			else
				err = 1; // Error for missing file
		}
		else if (redir->type == TOKEN_REDIRECT_IN)
		{
			if (redir->file)
				handle_redirections_in(redir->file, &err);
			else
				err = 1; // Error for missing file
		}
		else if (redir->type == TOKEN_APPEND)
		{
			if (redir->file)
				handle_redirections_append(redir->file, &err);
			else
				err = 1; // Error for missing file
		}
		else if (redir->type == TOKEN_HEREDOC)
		{
			args.state = state;
			// Always use heredoc_content if present
			if (redir->heredoc_content)
			{
				res = handle_redirections_heredoc_with_content(redir->heredoc_content, 
					redir->quoted, envp, state, &args);
			}
			else if (redir->file)
			{
				// If heredoc_content is missing, treat as empty heredoc
				char *empty_lines[1] = {NULL};
				res = handle_redirections_heredoc_with_content(empty_lines, redir->quoted, envp, state, &args);
			}
			else
			{
				err = 1; // Error for missing file/content
				res = 0;
			}
			if (res == 130)
				return (130);
			if (res != 0)
				err = 1;
		}
		if (err)
			return (1);
		redir = redir->next;
	}
	return (0);
}
