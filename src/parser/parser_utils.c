/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/07 22:17:57 by djuarez          ###   ########.fr       */
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

/*int process_token(t_cmd *cmd, t_token *cur, int *argc,
                  char **envp, t_exec_state *state)
{
    t_fragment *frag;
    char *arg;
    char *expanded;
    char *tmp;
    int skip;

    if (*argc >= MAX_ARGS - 1)
        return 0;

    arg = ft_strdup("");
    if (!arg)
        return 0;

    skip = should_skip_expansion(cur);

    frag = cur->fragments;
    while (frag)
    {
        if (skip)
            expanded = ft_strdup(frag->text);
        else
            expanded = expand_fragment_with_context(frag, cur, envp, state);

        if (!expanded)
        {
            free(arg);
            return 0;
        }

        tmp = str_append(arg, expanded);
        free(expanded);
        arg = tmp;
        if (!arg)
            return 0;

        frag = frag->next;
    }

    cmd->argv[*argc] = arg;
    cmd->argv_quote[*argc] = cur->fragments
        ? cur->fragments->quote_type
        : QUOTE_NONE;
    (*argc)++;
    return 1;
}*/

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
        {
            printf("DEBUG expand_fragment: text='%s' quote_type=%d -> EXPAND\n",
                   frag->text, frag->quote_type);

            // Llamamos a expand_variables con los 4 args correctos
            expanded = expand_variables(frag->text, envp, NULL, frag->quote_type);
        }
        else
        {
            printf("DEBUG expand_fragment: text='%s' quote_type=%d -> NO EXPAND\n",
                   frag->text, frag->quote_type);
            expanded = strdup(frag->text);
        }

        if (expanded)
        {
            argv[*argc] = expanded;
            printf("DEBUG process_token: argv[%d]='%s' quote=%d\n",
                   *argc, argv[*argc], frag->quote_type);
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

    // 1️⃣ Expandir fragments
    expand_fragments(tok, ctx->envp, ctx->state);

    // 2️⃣ Construir final_text
    free(tok->final_text);
    tok->final_text = concat_final_text(tok);
    printf("DEBUG FINAL_TEXT limpio (tok): '%s'\n", tok->final_text);

    // 3️⃣ Guardar en argv_final_text (una posición por token)
    if (ctx->cmd->argv_final_text)
    {
        ctx->cmd->argv_final_text[*ctx->argc_final_text] = ft_strdup(tok->final_text);
        printf("DEBUG CMD->ARGV_FINAL_TEXT[%d]: '%s'\n",
               *ctx->argc_final_text, ctx->cmd->argv_final_text[*ctx->argc_final_text]);
        (*ctx->argc_final_text)++;
    }

    // 4️⃣ Construir argv real (una posición por fragment)
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