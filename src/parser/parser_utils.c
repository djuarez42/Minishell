/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/12 16:54:09 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Free argv entries up to argc (if argc >= 0), and argv/argv_quote arrays.
   Does not free cmd itself. */
void	free_partial_cmd(t_cmd *cmd, int argc)
{
	int i;

	if (!cmd)
		return;
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

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;
	int		i;

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
		if (cmd->argv_quote)
			free(cmd->argv_quote);
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

int	init_cmd_args(t_cmd *cmd)
{
	cmd->argv = malloc(sizeof(char *) * MAX_ARGS);
	cmd->argv_quote = malloc(sizeof(t_quote_type) * MAX_ARGS);
	if (!cmd->argv || !cmd->argv_quote)
	{
		free(cmd->argv);
		free(cmd->argv_quote);
		return (0);
	}
	return (1);
}

char **process_token(t_token *tok, char **argv, int *argc, char **envp)
{
    t_fragment *frag;
    char *expanded;

    if (!tok)
        return argv;

    frag = tok->fragments;
    while (frag)
    {
        if (should_expand_fragment(frag))
            expanded = expand_variables(frag->text, envp, NULL, frag->quote_type);
        else
            expanded = strdup(frag->text);

        if (expanded)
        {
            argv[*argc] = expanded;
            (*argc)++;
        }

        frag = frag->next;
    }

    return argv;
}

char *concat_token_fragments(t_token *tok, char **envp, t_exec_state *state)
{
    t_fragment *frag = tok->fragments;
    char *result = ft_strdup("");
    char *tmp;

    if (!result)
        return NULL;

    while (frag)
    {
        // Ignorar fragments vacíos
        if (frag->text[0] == '\0')
        {
            frag = frag->next;
            continue;
        }

        // Expande variables solo si corresponde
        if (should_expand_fragment(frag))
            tmp = expand_variables(frag->text, envp, state, frag->quote_type);
        else
            tmp = ft_strdup(frag->text);

        if (tmp)
        {
            // Concatenar al resultado final_text
            char *new_result = str_append(result, tmp); // str_append libera result internamente
            free(tmp);
            result = new_result;
        }

        frag = frag->next;
    }

    return result;
}

char **process_token_with_quotes(t_token *tok, t_proc_ctx *ctx)
{
    t_fragment *frag;
    int is_assignment = 0;

    if (!tok)
        return ctx->cmd->argv;

    // 1️⃣ Expandir fragments
    expand_fragments(tok, ctx->envp, ctx->state);

    // 2️⃣ Construir final_text
    free(tok->final_text);
    tok->final_text = concat_final_text(tok);

    // 3️⃣ Guardar final_text en argv_final_text
    if (ctx->cmd->argv_final_text)
    {
        ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup(tok->final_text);
        (*ctx->argc_final_text)++;
    }

    // 4️⃣ Detectar si es assignment (mirando solo el primer fragment con '=')
    frag = tok->fragments;
    if (frag && ft_strchr(frag->text, '='))
        is_assignment = 1;

    // 5️⃣ Construir argv
    if (is_assignment)
    {
        // Concatenar todos los fragments del token en un solo argv
        ctx->cmd->argv[*ctx->argc_argv] = ft_strdup(tok->final_text);
        ctx->cmd->argv_quote[*ctx->argc_argv] = QUOTE_NONE;
        (*ctx->argc_argv)++;
    }
    else
    {
        // Fragmentos normales: aplicar split según quotes
        frag = tok->fragments;
        while (frag)
        {
            if (frag->expanded_text)
            {
                char **words = split_for_argv(frag->expanded_text, frag->quote_type);
                for (int i = 0; words && words[i]; i++)
                {
                    ctx->cmd->argv[*ctx->argc_argv] = ft_strdup(words[i]);
                    ctx->cmd->argv_quote[*ctx->argc_argv] = frag->quote_type;
                    (*ctx->argc_argv)++;
                }
                free_str_array(words);
            }
            frag = frag->next;
        }
    }

    return ctx->cmd->argv;
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

char **split_for_argv(const char *text, int quote_type)
{
    if (quote_type != QUOTE_NONE)
    {
        // fragment con quotes: se conserva como está
        char **res = malloc(sizeof(char *) * 2);
        if (!res)
            return NULL;
        res[0] = ft_strdup(text);
        res[1] = NULL;
        return res;
    }
    else
    {
        // fragment no-quoted: aplicamos word splitting
        return ft_split_spaces(text);
    }
}


char **ft_split_spaces(const char *s)
{
    int count = 0;
    int in_word = 0;
    const char *p = s;
    char **res;
    int w;
    int len;

    if (!s)
        return NULL;

    // contar palabras
    while (*p)
    {
        if (!isspace((unsigned char)*p))
        {
            if (!in_word)
            {
                count++;
                in_word = 1;
            }
        }
        else
            in_word = 0;
        p++;
    }

    res = malloc(sizeof(char *) * (count + 1));
    if (!res)
        return NULL;

    // copiar palabras
    w = 0;
    while (*s)
    {
        while (*s && isspace((unsigned char)*s))
            s++;
        if (!*s)
            break;

        len = 0;
        while (s[len] && !isspace((unsigned char)s[len]))
            len++;

        res[w++] = ft_strndup(s, len);
        s += len;
    }
    res[w] = NULL;
    return res;
}