/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/13 18:57:10 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* forward declare to avoid header ordering issues */
void	free_partial_cmd(t_cmd *cmd, int argc);

t_cmd *parser_tokens(t_token *tokens, char **envp, t_exec_state *state)
{
    t_cmd *head;
    t_cmd *last;
    t_token *cur;
    t_cmd *new_cmd;

    head = NULL;
    last = NULL;
    cur = tokens;

    if (cur && cur->type == TOKEN_PIPE)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        return (NULL);
    }

    while (cur && cur->type != TOKEN_EOF)
    {
        if (cur->type == TOKEN_PIPE && cur->fragments &&
            ft_strlen(cur->fragments->text) > 1)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
            return (NULL);
        }

        new_cmd = create_cmd_node(&cur, envp, state);
        if (!new_cmd)
            return (free_cmds(head), NULL);
        add_cmd_node(&head, &last, new_cmd);
    }
    return (head);
}

t_token *parse_cmd_block(t_token *cur, t_cmd *cmd,
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
            break;
    }
    return (cur);
}


t_token	*parse_redirections(t_token *cur, t_cmd *cmd)
{
	t_redir	*new_redir;
	t_redir	*last;

	if (!cur)
		return (NULL);
	if (!cur->next || cur->next->type != TOKEN_WORD)
		return (NULL);

	new_redir = create_redir(cur);
	if (!new_redir)
		return (NULL);

	if (!cmd->redirs)
		cmd->redirs = new_redir;
	else
	{
		last = cmd->redirs;
		while (last->next)
			last = last->next;
		last->next = new_redir;
	}
	return (cur->next->next);
}


t_token *parse_arguments(t_token *cur, t_cmd *cmd,
                         char **envp, t_exec_state *state)
{
    int argc_argv = 0;
    int argc_final_text = 0;
    t_proc_ctx ctx;

    ctx.cmd = cmd;
    ctx.argc_argv = &argc_argv;
    ctx.argc_final_text = &argc_final_text;
    ctx.envp = envp;
    ctx.state = state;

    while (cur && cur->type == TOKEN_WORD)
    {
        cmd->argv = process_token_with_quotes(cur, &ctx);
        if (!cmd->argv)
        {
            free_partial_cmd(cmd, argc_argv);
            return NULL;
        }
        cur = cur->next;
    }

    // 🔹 Poner NULL terminators
    cmd->argv[argc_argv] = NULL;
    cmd->argv_quote[argc_argv] = QUOTE_NONE;
    if (cmd->argv_final_text)
        cmd->argv_final_text[argc_final_text] = NULL;

    return cur;
}

char *expand_fragment(const char *text, t_quote_type quote,
                      char **envp, t_exec_state *state)
{
    if (!text)
        return ft_strdup("");

    if (quote == QUOTE_SINGLE)
        return ft_strdup(text);

    if (quote == QUOTE_DOLLAR)
        return expand_ansi_c_string(text); // tu función ya existente

    if (text[0] == '~' && quote == QUOTE_NONE)
        return expand_tilde_bash(text, envp);

    return expand_variables(text, envp, state, quote);
}


void expand_fragments(t_token *tok, char **envp, t_exec_state *state)
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

char	*expand_tilde_bash(const char *text, char **envp)
{
	const char	*home;
	char		*suffix;
	char		*res;

	if (!text || text[0] != '~')
		return (ft_strdup(text));

	/*
	** Casos válidos para expansión de ~ en Bash:
	**   ~        -> $HOME
	**   ~/algo   -> $HOME/algo
	**
	** Casos NO válidos (se dejan literales):
	**   ~usuario -> sin soporte en Minishell
	**   ~42      -> se deja como "~42"
	*/
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

t_token *expand_token_to_tokens(t_token *tok, char **envp, t_exec_state *state)
{
    char **words;
    int word_count = 0;
    t_token *head = NULL;
    t_token *last = NULL;

    if (!tok)
        return NULL;

    // 1) Expandir fragments del token como siempre
    expand_fragments(tok, envp, state);

    // 2) Construir palabras a partir del token
    words = build_words_from_token(tok, &word_count);

    if (!words)
        return NULL;

    // 3) Por cada palabra, crear un token
    for (int i = 0; i < word_count; i++)
    {
        t_token_type type = TOKEN_WORD;

        // Detectar operadores
        if (strcmp(words[i], "|") == 0)
            type = TOKEN_PIPE;
        else if (strcmp(words[i], "<") == 0)
            type = TOKEN_REDIRECT_IN;
        else if (strcmp(words[i], ">") == 0)
            type = TOKEN_REDIRECT_OUT;
        else if (strcmp(words[i], "<<") == 0)
            type = TOKEN_HEREDOC;
        else if (strcmp(words[i], ">>") == 0)
            type = TOKEN_APPEND;

        t_token *new_tok = create_token(type, tok->has_space_before);
        new_tok->fragments = new_fragment(words[i], strlen(words[i]), QUOTE_NONE, false);
        new_tok->final_text = ft_strdup(words[i]);

        // Append a la lista
        if (!head)
            head = new_tok;
        else
            last->next = new_tok;
        last = new_tok;
    }

    free_str_array(words);
    return head;
}