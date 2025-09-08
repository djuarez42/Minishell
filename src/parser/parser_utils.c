/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/08 18:45:48 by ekakhmad         ###   ########.fr       */
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
		if (cmd->argv_final_text)
		{
			i = 0;
			while (cmd->argv_final_text[i])
				free(cmd->argv_final_text[i++]);
			free(cmd->argv_final_text);
		}
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

    if (!tok)
        return ctx->cmd->argv;

    // 1️⃣ Expand fragments - converts $VARS and handles quotes
    expand_fragments(tok, ctx->envp, ctx->state);

    // 2️⃣ Build final_text - concatenates all fragments WITHOUT spaces between them
    // This ensures '"'$USER'"' becomes "ekakhmad" and not " ekakhmad "
    free(tok->final_text);
    tok->final_text = concat_final_text(tok);

    // 3️⃣ Store in argv_final_text (one entry per token)
    // This is used for external commands and echo to preserve correct quoting
    if (ctx->cmd->argv_final_text)
    {
        ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup(tok->final_text);
        (*ctx->argc_final_text)++;
    }

    // 4️⃣ Build regular argv (one entry per fragment)
    // This is kept for compatibility but may add unwanted spaces between fragments
    frag = tok->fragments;
    while (frag)
    {
        if (frag->expanded_text)
        {
            ctx->cmd->argv[*ctx->argc_argv] = ft_strdup(frag->expanded_text);
            ctx->cmd->argv_quote[*ctx->argc_argv] = frag->quote_type;
            (*ctx->argc_argv)++;
        }
        frag = frag->next;
    }

    return ctx->cmd->argv;
}