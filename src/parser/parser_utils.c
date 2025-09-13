/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/13 15:52:50 by ekakhmad         ###   ########.fr       */
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
		else
		{
			/* argc < 0 signals: free all entries until NULL */
			i = 0;
			while (cmd->argv[i])
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
    if (!tok)
        return ctx->cmd->argv;

    // 1️⃣ Expandir fragments (se siguen expandiendo como antes)
    expand_fragments(tok, ctx->envp, ctx->state);

    // 2️⃣ Construir final_text a partir de los fragments
    free(tok->final_text);
    tok->final_text = concat_final_text(tok);

	/* If there are no quoted fragments in this token and the final_text
	 * contains unexpanded '$', expand the whole final_text at once. This
	 * fixes cases where parse_mixed_fragments produced separate fragments
	 * like '$' and 'NAME' causing per-fragment expansion to miss variables. */
	{
		bool any_quoted = false;
		t_fragment *ff = tok->fragments;
		while (ff)
		{
			if (ff->quote_type != QUOTE_NONE)
			{
				any_quoted = true;
				break;
			}
			ff = ff->next;
		}
		if (!any_quoted && ft_strchr(tok->final_text, '$'))
		{
			char *expanded_all = expand_variables(tok->final_text, ctx->envp, ctx->state, QUOTE_NONE);
			if (expanded_all)
			{
				free(tok->final_text);
				tok->final_text = expanded_all;
			}
		}
	}

	/* 3️⃣ NOTE: we must not append the raw tok->final_text to argv_final_text
	 * before performing word-splitting. argv_final_text must match the
	 * post-splitting argv that will be executed; appending early would leave
	 * an incorrect entry (e.g. "/bin/echo 42") as argv_final_text[0]. */

	// 4️⃣ Guardar en argv
	bool has_quoted_fragment = false;
	t_fragment *f = tok->fragments;
	while (f)
	{
		if (f->quote_type != QUOTE_NONE)
		{
			has_quoted_fragment = true;
			break;
		}
		f = f->next;
	}

	if (tok->final_text && *tok->final_text)
	{
		// (existing logic unchanged)
		bool has_unquoted_fragment = false;
		bool is_assignment_with_quoted_value = false;
		t_fragment *f2 = tok->fragments;
		bool found_eq = false;
		while (f2)
		{
			if (f2->quote_type == QUOTE_NONE)
				has_unquoted_fragment = true;
			if (!found_eq)
			{
				char *eq = ft_strchr(f2->text, '=');
				if (eq)
				{
					found_eq = true;
					if (*(eq + 1) != '\0')
					{
						if (f2->quote_type != QUOTE_NONE)
							is_assignment_with_quoted_value = true;
					}
					else
					{
						t_fragment *nf = f2->next;
						if (nf && nf->quote_type != QUOTE_NONE)
							is_assignment_with_quoted_value = true;
					}
				}
			}
			f2 = f2->next;
		}
		bool contains_ifs = false;
		for (char *p = tok->final_text; *p; ++p)
			if (*p == ' ' || *p == '\t' || *p == '\n') { contains_ifs = true; break; }
		if (contains_ifs && has_unquoted_fragment && !is_assignment_with_quoted_value)
		{
			const char *s = tok->final_text;
			int len = ft_strlen(s);
			int i = 0;
			while (i < len)
			{
				while (i < len && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
					i++;
				if (i >= len) break;
				int start = i;
				while (i < len && !(s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
					i++;
				int field_len = i - start;
				if (field_len > 0)
				{
					char *field = malloc((size_t)field_len + 1);
					if (!field)
						return ctx->cmd->argv;
					ft_strlcpy(field, &s[start], (size_t)field_len + 1);
					if (ctx->cmd->argv_final_text)
					{
						ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup(field);
						(*ctx->argc_final_text)++;
					}
					ctx->cmd->argv[*ctx->argc_argv] = field;
					ctx->cmd->argv_quote[*ctx->argc_argv] = QUOTE_NONE;
					(*ctx->argc_argv)++;
				}
			}
		}
		else
		{
			ctx->cmd->argv[*ctx->argc_argv] = ft_strdup(tok->final_text);
			if (ctx->cmd->argv_final_text)
			{
				ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup(tok->final_text);
				(*ctx->argc_final_text)++;
			}
			ctx->cmd->argv_quote[*ctx->argc_argv] = QUOTE_NONE;
			(*ctx->argc_argv)++;
		}
	}
	else if (has_quoted_fragment)
	{
		// Add empty quoted argument to argv_final_text
		ctx->cmd->argv[*ctx->argc_argv] = ft_strdup("");
		if (ctx->cmd->argv_final_text)
		{
			ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup("");
			(*ctx->argc_final_text)++;
		}
		ctx->cmd->argv_quote[*ctx->argc_argv] = QUOTE_NONE;
		(*ctx->argc_argv)++;
	}

    return ctx->cmd->argv;
}

/**
 * Modified parse_arguments-style helper that consumes WORD tokens and appends
 * to cmd->argv using persistent argc counters provided by the caller.
 * This function mirrors the previous behavior but allows being called
 * multiple times so arguments accumulate across redirections.
 */
t_token *parse_arguments(t_token *cur, t_cmd *cmd,
						 char **envp, t_exec_state *state,
						 int *p_argc_argv, int *p_argc_final_text)
{
	t_proc_ctx ctx;

	if (!p_argc_argv || !p_argc_final_text)
		return NULL;

	ctx.cmd = cmd;
	ctx.argc_argv = p_argc_argv;
	ctx.argc_final_text = p_argc_final_text;
	ctx.envp = envp;
	ctx.state = state;

	while (cur && cur->type == TOKEN_WORD)
	{
		cmd->argv = process_token_with_quotes(cur, &ctx);
		if (!cmd->argv)
		{
			free_partial_cmd(cmd, *p_argc_argv);
			return NULL;
		}
		cur = cur->next;
	}

	/* Ensure terminators are set by the caller eventually */
	cmd->argv[*p_argc_argv] = NULL;
	cmd->argv_quote[*p_argc_argv] = QUOTE_NONE;
	if (cmd->argv_final_text)
		cmd->argv_final_text[*p_argc_final_text] = NULL;

	return cur;
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

void free_redirs(t_redir *redir)
{
	t_redir *next;

	while (redir)
	{
		next = redir->next;
		if (redir->file)
			free(redir->file);
		if (redir->heredoc_content)
			free_str_array(redir->heredoc_content);
		free(redir);
		redir = next;
	}
}

void free_cmds(t_cmd *cmd)
{
	t_cmd *next;

	while (cmd)
	{
		next = cmd->next;
		if (cmd->redirs)
			free_redirs(cmd->redirs);
		if (cmd->argv_final_text)
			free_str_array(cmd->argv_final_text);
		free_partial_cmd(cmd, -1);
		free(cmd);
		cmd = next;
	}
}