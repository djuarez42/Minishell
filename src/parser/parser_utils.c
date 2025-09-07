/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/07 02:29:37 by djuarez          ###   ########.fr       */
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
//versiones que no expanden
/*char **process_token_with_quotes(t_token *tok, t_proc_ctx *ctx)
{
    t_fragment *frag;
    char *tmp;
    char *display_buf;

    if (!tok)
        return ctx->cmd->argv;

    display_buf = concat_token_fragments(tok, ctx->envp, ctx->state);
    if (!display_buf)
        display_buf = ft_strdup(""); // fallback

    frag = tok->fragments;
    while (frag)
    {
        if (should_expand_fragment(frag))
            tmp = expand_variables(frag->text, ctx->envp, ctx->state, frag->quote_type);
        else
            tmp = ft_strdup(frag->text);

        if (tmp)
        {
            // Solo prints para debug
            printf("DEBUG FRAGMENT: text='%s' quote=%d tmp='%s'\n",
                   frag->text, frag->quote_type, tmp);

            ctx->cmd->argv[*ctx->argc] = tmp;
            ctx->cmd->argv_quote[*ctx->argc] = frag->quote_type;
            (*ctx->argc)++;

            printf("DEBUG display_buf antes append: '%s'\n", display_buf);
            char *new_display = str_append(display_buf, tmp);
            printf("DEBUG display_buf despues append: '%s'\n", new_display);

            display_buf = new_display;
        }

        frag = frag->next;
    }

    printf("DEBUG final_text antes asignar: '%s'\n", display_buf);
    free(tok->final_text);
    tok->final_text = display_buf;
    printf("DEBUG tok->final_text asignado: '%s'\n", tok->final_text);

    return ctx->cmd->argv;
}

char *concat_token_fragments(t_token *tok, char **envp, t_exec_state *state)
{
    t_fragment *frag = tok->fragments;
    char *result = ft_strdup("");
    char *tmp;

    while (frag)
    {
        if (frag->quote_type == QUOTE_SINGLE && frag->text[0] == '\0')
        {
            printf("DEBUG SKIP EMPTY SINGLE QUOTE FRAGMENT\n");
            frag = frag->next;
            continue;
        }

        if (should_expand_fragment(frag))
            tmp = expand_variables(frag->text, envp, state, frag->quote_type);
        else
            tmp = ft_strdup(frag->text);

        printf("DEBUG CONCAT FRAGMENT: text='%s' quote=%d tmp='%s'\n",
               frag->text, frag->quote_type, tmp ? tmp : "(NULL)");

        if (tmp)
        {
            result = str_append(result, tmp); // concatenación normal
            free(tmp);
        }

        frag = frag->next;
    }

    printf("DEBUG CONCAT FINAL RESULT: '%s'\n", result);
    return result;
}*/
// versiones que expanden 
/*char *concat_token_fragments(t_token *tok, char **envp, t_exec_state *state)
{
    t_fragment *frag;
    char *result;
    char *piece;

    if (!tok)
        return NULL;
    result = ft_strdup("");
    if (!result)
        return NULL;

    frag = tok->fragments;
    while (frag)
    {
       
        if (!frag->text || frag->text[0] == '\0')
        {
            frag = frag->next;
            continue;
        }

       
        if (frag->quote_type == QUOTE_SINGLE)
            piece = ft_strdup(frag->text);
        else if (should_expand_fragment(frag))
            piece = expand_variables(frag->text, envp, state, frag->quote_type);
        else
            piece = ft_strdup(frag->text);

        if (!piece)
        {
            free(result);
            return NULL;
        }

       
        result = str_append(result, piece);
        free(piece);

        frag = frag->next;
    }

    return result;
}*/
/*
char **process_token_with_quotes(t_token *tok, t_proc_ctx *ctx)
{
    char *final_text;
    int idx;

    if (!tok || !ctx || !ctx->cmd || !ctx->argc)
        return ctx ? ctx->cmd->argv : NULL;

    
    final_text = concat_token_fragments(tok, ctx->envp, ctx->state);
    if (!final_text)
        final_text = ft_strdup("");

    
    idx = *ctx->argc;
    ctx->cmd->argv[idx] = ft_strdup(final_text);
    
    ctx->cmd->argv_quote[idx] = (tok->fragments ? tok->fragments->quote_type : QUOTE_NONE);
    (*ctx->argc)++;

    
    if (tok->final_text)
        free(tok->final_text);
    tok->final_text = final_text; 

    return ctx->cmd->argv;
}
*/


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
    char *tmp;
    char *display_buf;

    if (!tok)
        return ctx->cmd->argv;

    // Construir final_text limpio
    display_buf = concat_token_fragments(tok, ctx->envp, ctx->state);
    if (!display_buf)
        display_buf = ft_strdup(""); // fallback

    // Guardar final_text en token
    free(tok->final_text);
    tok->final_text = display_buf;
	printf("DEBUG FINAL_TEXT limpio: '%s'\n", tok->final_text);

    // Construir argv fragmentado, respetando quotes y sin modificar final_text
    frag = tok->fragments;
    while (frag)
    {
        if (frag->text[0] != '\0')
        {
            if (should_expand_fragment(frag))
                tmp = expand_variables(frag->text, ctx->envp, ctx->state, frag->quote_type);
            else
                tmp = ft_strdup(frag->text);

            if (tmp)
            {
                ctx->cmd->argv[*ctx->argc] = tmp;
                ctx->cmd->argv_quote[*ctx->argc] = frag->quote_type;
                (*ctx->argc)++;
            }
        }
        frag = frag->next;
    }

    return ctx->cmd->argv;
}
