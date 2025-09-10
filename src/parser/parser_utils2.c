/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 21:21:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/10 18:47:46 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redir *create_redir(t_token *cur)
{
    t_redir *redir;
    t_fragment *frag;

    if (!cur || !cur->next || !cur->next->fragments)
        return (NULL);

    redir = malloc(sizeof(t_redir));
    if (!redir)
        return (NULL);

    redir->type = cur->type;

    // Guardar fragments completos para acceder a quote_type y demás
    redir->fragments = cur->next->fragments;

    // Detectar si el heredoc estaba entre comillas
    frag = redir->fragments;
    redir->quoted = false;
    while (frag)
    {
        if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
        {
            redir->quoted = true;
            break;
        }
        frag = frag->next;
    }

    // Reconstruir el file / delimiter usando todos los fragments del token
    {
        t_token dummy_tok;
        dummy_tok.fragments = cur->next->fragments;
        redir->file = concat_final_text(&dummy_tok);
        if (!redir->file)
        {
            free(redir);
            return (NULL);
        }
    }

    // Para heredoc, opcionalmente recolectar contenido
    if (redir->type == TOKEN_HEREDOC)
    {
        redir->heredoc_content = collect_heredoc_content(redir->file, redir->quoted);
        if (!redir->heredoc_content)
        {
            free(redir->file);
            free(redir);
            return (NULL);
        }
    }
    else
        redir->heredoc_content = NULL;

    redir->next = NULL;
    return (redir);
}


bool	is_quoted(const char *str)
{
	int	len;

	len = ft_strlen(str);
	if (len >= 2)
	{
		if ((str[0] == '"' && str[len - 1] == '"')
			|| (str[0] == '\'' && str[len - 1] == '\''))
			return (true);
	}
	return (false);
}

void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*last)->next = new_cmd;
	*last = new_cmd;
}

int	add_argument(t_cmd *cmd, char *value, t_quote_type quote, int *argc)
{
	cmd->argv[*argc] = ft_strdup(value);
	if (!cmd->argv[*argc])
	{
		while (--(*argc) >= 0)
			free(cmd->argv[*argc]);
		free(cmd->argv);
		cmd->argv = NULL;
		return (0);
	}
	cmd->argv_quote[*argc] = quote;
	(*argc)++;
	return (1);
}

t_cmd *create_cmd_node(t_token **cur, char **envp, t_exec_state *state)
{
    t_cmd *cmd;

    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return NULL;

    cmd->argv = malloc(sizeof(char *) * MAX_ARGS);
    cmd->argv_quote = malloc(sizeof(int) * MAX_ARGS);
    cmd->argv_final_text = malloc(sizeof(char *) * MAX_ARGS); // reservar también
    if (!cmd->argv || !cmd->argv_quote || !cmd->argv_final_text)
    {
        free(cmd->argv);
        free(cmd->argv_quote);
        free(cmd->argv_final_text);
        free(cmd);
        return NULL;
    }

    for (int i = 0; i < MAX_ARGS; i++)
    {
        cmd->argv[i] = NULL;
        cmd->argv_quote[i] = QUOTE_NONE;
        cmd->argv_final_text[i] = NULL; // inicializar
    }

    cmd->redirs = NULL;
    cmd->pipe = 0;
    cmd->next = NULL;

    *cur = parse_cmd_block(*cur, cmd, envp, state);
    if (!*cur)
    {
        free(cmd->argv);
        free(cmd->argv_quote);
        free(cmd->argv_final_text);
        free(cmd);
        return NULL;
    }

    if ((*cur)->type == TOKEN_PIPE)
    {
        cmd->pipe = 1;
        *cur = (*cur)->next;
    }

    return cmd;
}

char	**collect_heredoc_content(const char *delimiter, bool quoted __attribute__((unused)))
{
	char	**lines;
	char	*line;
	int		capacity;
	int		count;
	int		is_interactive;

	is_interactive = isatty(STDIN_FILENO);
	capacity = 10;
	count = 0;
	lines = malloc(sizeof(char *) * capacity);
	if (!lines)
		return (NULL);

	while (1)
	{
		if (is_interactive)
		{
			line = readline("> ");
		}
		else
		{
			// For non-interactive mode, read directly from stdin
			char *buffer = NULL;
			size_t len = 0;
			ssize_t read_len = getline(&buffer, &len, stdin);
			if (read_len == -1)
			{
				line = NULL;
			}
			else
			{
				// Remove trailing newline if present
				if (read_len > 0 && buffer[read_len - 1] == '\n')
					buffer[read_len - 1] = '\0';
				line = ft_strdup(buffer);
				free(buffer);
			}
		}
		
		if (!line)
		{
			// EOF - free collected lines and return NULL
			while (count > 0)
				free(lines[--count]);
			free(lines);
			return (NULL);
		}
		
		// Check if this is the delimiter
		if ((ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
				&& line[ft_strlen(delimiter)] == '\0'))
		{
			free(line);
			break ;
		}
		
		// Expand lines array if needed
		if (count >= capacity - 1)
		{
			capacity *= 2;
			char **new_lines = malloc(sizeof(char *) * capacity);
			int i;
			if (!new_lines)
			{
				free(line);
				while (count > 0)
					free(lines[--count]);
				free(lines);
				return (NULL);
			}
			for (i = 0; i < count; i++)
				new_lines[i] = lines[i];
			free(lines);
			lines = new_lines;
		}
		
		lines[count++] = line;
	}
	
	lines[count] = NULL;  // Null terminate the array
	return (lines);
}
