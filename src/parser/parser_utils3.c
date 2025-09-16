/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 02:39:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/16 19:49:08 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_tilde_bash(const char *text, char **envp)
{
	const char	*home;
	char		*suffix;
	char		*res;

	if (!text || text[0] != '~')
		return (ft_strdup(text));
	if (text[1] && text[1] != '/')
		return (ft_strdup(text));
	home = env_get_value(envp, "HOME");
	if (!home)
		home = "";
	suffix = ft_strdup(text + 1);
	if (!suffix)
		return (NULL);
	res = ft_strjoin(home, suffix);
	free(suffix);
	return (res);
}

char *concat_final_text(t_token *tok)
{
    t_fragment *frag = tok->fragments;
    char *final_text = ft_strdup("");
    char *tmp;
    char *new_result;

    while (frag)
    {
        if (frag->expanded_text)
            tmp = ft_strdup(frag->expanded_text);
        else
            tmp = ft_strdup(frag->text);

        new_result = str_append(final_text, tmp);
        free(tmp);
        final_text = new_result;

        frag = frag->next;
    }

    return final_text;
}

int should_expand_fragment(t_fragment *frag)
{
    int i;

    if (!frag || !frag->text)
        return 0;

    /* Nunca expandir dentro de comillas simples */
    if (frag->quote_type == QUOTE_SINGLE)
        return 0;

    /* Casos especiales: comillas dobles y texto sin variables */
    if (frag->quote_type == QUOTE_DOUBLE || frag->quote_type == QUOTE_NONE)
    {
        i = 0;
        while (frag->text[i])
        {
            if (frag->text[i] == '$')
                return 1; /* expandir variables */
            if (frag->text[i] == '~' && i == 0 && frag->quote_type == QUOTE_NONE)
                return 1; /* tilde solo si no hay comillas */
            i++;
        }
        return 0; /* nada que expandir, literal */
    }

    if (frag->quote_type == QUOTE_DOLLAR)
        return 1; /* $'ansi' siempre se interpreta */

    return 0;
}

char *expand_fragment(const char *text, t_quote_type quote,
					  char **envp, t_exec_state *state)
{
	if (!text)
		return ft_strdup("");
	if (quote == QUOTE_SINGLE)
		return ft_strdup(text);
	if (quote == QUOTE_DOLLAR)
		return expand_ansi_c_string(text);
	if (text[0] == '~' && quote == QUOTE_NONE)
		return expand_tilde_bash(text, envp);
	return expand_variables(text, envp, state, quote);
}

void    expand_fragments(t_token *tok, char **envp, t_exec_state *state)
{
	t_fragment *frag = tok->fragments;

	while (frag)
	{
		if (should_expand_fragment(frag))
			frag->expanded_text = expand_fragment(frag->text,
												  frag->quote_type,
												  envp,
												  state);
		else
			frag->expanded_text = ft_strdup(frag->text);
		frag = frag->next;
	}
}

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

void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*last)->next = new_cmd;
	*last = new_cmd;
}

t_cmd *create_cmd_node(t_token **cur, char **envp, t_exec_state *state)
{
    t_cmd *cmd;

    cmd = malloc(sizeof(t_cmd));
    if (!cmd)
        return NULL;

    cmd->argv = malloc(sizeof(char *) * MAX_ARGS);
    cmd->argv_quote = malloc(sizeof(t_quote_type) * MAX_ARGS);
    cmd->argv_final_text = malloc(sizeof(char *) * MAX_ARGS);
    cmd->argv_first_word = malloc(sizeof(bool) * MAX_ARGS); // 🔹 nueva

    if (!cmd->argv || !cmd->argv_quote || !cmd->argv_final_text || !cmd->argv_first_word)
    {
        free(cmd->argv);
        free(cmd->argv_quote);
        free(cmd->argv_final_text);
        free(cmd->argv_first_word); // 🔹 liberar también
        free(cmd);
        return NULL;
    }

    for (int i = 0; i < MAX_ARGS; i++)
    {
        cmd->argv[i] = NULL;
        cmd->argv_quote[i] = QUOTE_NONE;
        cmd->argv_final_text[i] = NULL;
        cmd->argv_first_word[i] = false; // 🔹 inicializar
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
        free(cmd->argv_first_word);
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

t_token	*parse_cmd_block(t_token *cur, t_cmd *cmd,
						char **envp, t_exec_state *state)
{
	cur = parse_arguments(cur, cmd, envp, state);
	if (!cur)
		return (NULL);
	while (cur && (cur->type == TOKEN_REDIRECT_OUT
			|| cur->type == TOKEN_REDIRECT_IN
			|| cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC))
	{
		cur = parse_redirections(cur, cmd);
		if (!cur)
		{
			free_partial_cmd(cmd, -1);
			free_redirs(cmd->redirs);
			cmd->redirs = NULL;
			return (NULL);
		}
		if (cur && cur->type == TOKEN_WORD)
			break ;
	}
	return (cur);
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

void	free_partial_cmd(t_cmd *cmd, int argc)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		if (argc >= 0)
		{
			i = 0;
			while (i < argc)
			{
				free(cmd->argv[i]);
				i++;
			}
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	if (cmd->argv_quote)
	{
		free(cmd->argv_quote);
		cmd->argv_quote = NULL;
	}
}

void free_cmds(t_cmd *cmd)
{
    t_cmd *tmp;
    int i;

    while (cmd)
    {
        tmp = cmd->next;
        if (cmd->argv)
        {
            i = 0;
            while (cmd->argv[i])
                free(cmd->argv[i++]);
            free(cmd->argv);
        }
        if (cmd->argv_final_text)
        {
            i = 0;
            while (cmd->argv_final_text[i])
                free(cmd->argv_final_text[i++]);
            free(cmd->argv_final_text);
        }
        if (cmd->argv_quote)
            free(cmd->argv_quote);
        if (cmd->argv_first_word)
            free(cmd->argv_first_word);
        if (cmd->redirs)
            free_redirs(cmd->redirs);
        free(cmd);
        cmd = tmp;
    }
}

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;
	int		i;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		if (redir->heredoc_content)
		{
			i = 0;
			while (redir->heredoc_content[i])
				free(redir->heredoc_content[i++]);
			free(redir->heredoc_content);
		}
		free(redir);
		redir = tmp;
	}
}

void free_str_array(char **arr)
{
    int i = 0;
    if (!arr)
        return;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

char **build_words_from_token(t_token *tok, int *out_count)
{
    t_fragment *frag;
    size_t total_len = 0;
    size_t pos = 0;
    char *S;
    unsigned char *splittable; 
    char **words = NULL;
    int count = 0;
    size_t i;

    if (!tok || !out_count)
        return NULL;
    frag = tok->fragments;
    while (frag)
    {
        if (frag->expanded_text)
            total_len += ft_strlen(frag->expanded_text);
        frag = frag->next;
    }
    S = malloc(total_len + 1);
    splittable = malloc(total_len);
    if (!S || !splittable)
    {
        free(S);
        free(splittable);
        *out_count = 0;
        return NULL;
    }
    frag = tok->fragments;
    pos = 0;
    while (frag)
    {
        if (frag->expanded_text)
        {
            size_t flen = ft_strlen(frag->expanded_text);
            int frag_splittable = 0;
            if (frag->quote_type == QUOTE_NONE && frag->text && frag->text[0] == '$')
                frag_splittable = 1;

            for (i = 0; i < flen; i++)
            {
                S[pos] = frag->expanded_text[i];
                splittable[pos] = frag_splittable ? 1 : 0;
                pos++;
            }
        }
        frag = frag->next;
    }
    S[pos] = '\0';
    i = 0;
    while (i < pos)
    {
        while (i < pos && ft_isspace((unsigned char)S[i]) && splittable[i])
            i++;
        if (i >= pos)
            break;
        count++;
        while (i < pos && !(ft_isspace((unsigned char)S[i]) && splittable[i]))
            i++;
    }
    if (count == 0)
    {
        int frag_count = 0;
        frag = tok->fragments;
        while (frag)
        {
            if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
                frag_count++;
            frag = frag->next;
        }
        if (frag_count > 0)
        {
            words = malloc(sizeof(char *) * (frag_count + 1));
            if (!words)
            {
                free(S);
                free(splittable);
                *out_count = 0;
                return NULL;
            }
            frag = tok->fragments;
            int wi = 0;
            while (frag)
            {
                if (frag->quote_type == QUOTE_SINGLE || frag->quote_type == QUOTE_DOUBLE)
                {
                    words[wi] = ft_strdup("");
                    wi++;
                }
                frag = frag->next;
            }
            words[frag_count] = NULL;
            *out_count = frag_count;
            free(S);
            free(splittable);
            return words;
        }
        free(S);
        free(splittable);
        *out_count = 0;
        return NULL;
    }
    words = malloc(sizeof(char *) * (count + 1));
    if (!words)
    {
        free(S);
        free(splittable);
        *out_count = 0;
        return NULL;
    }
    i = 0;
    int wi = 0;
    while (i < pos && wi < count)
    {
        while (i < pos && ft_isspace((unsigned char)S[i]) && splittable[i])
            i++;
        if (i >= pos)
            break;
        size_t start = i;
        while (i < pos && !(ft_isspace((unsigned char)S[i]) && splittable[i]))
            i++;
        size_t wlen = i - start;
        words[wi] = ft_strndup(&S[start], wlen);
        wi++;
    }
    words[wi] = NULL;
    free(S);
    free(splittable);
    *out_count = count;
    return words;
}

void    update_final_text(t_token *tok, t_proc_ctx *ctx)
{
	expand_fragments(tok, ctx->envp, ctx->state);
	free(tok->final_text);
	tok->final_text = concat_final_text(tok);
}
