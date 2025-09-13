/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/13 00:14:24 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* forward declare to avoid header ordering issues */
void	free_partial_cmd(t_cmd *cmd, int argc);
// Use the global environment pointer defined in main.c
extern char **g_envp;

t_cmd *parser_tokens(t_token *tokens, char **envp, t_exec_state *state)
{
    t_cmd *head;
    t_cmd *last;
    t_token *cur;
    t_cmd *new_cmd;

    head = NULL;
    last = NULL;
    cur = tokens;

    /* ensure global env pointer is initialized for parser helpers */
    g_envp = envp;

    if (cur && cur->type == TOKEN_PIPE)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
        extern int g_exit_code;
        g_exit_code = 2;
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
    int argc_argv = 0;
    int argc_final_text = 0;
    cur = parse_arguments(cur, cmd, envp, state, &argc_argv, &argc_final_text);
    if (!cur)
        return (NULL);
    while (cur && (cur->type == TOKEN_REDIRECT_OUT
            || cur->type == TOKEN_REDIRECT_IN
            || cur->type == TOKEN_APPEND
            || cur->type == TOKEN_HEREDOC))
    {
        cur = parse_redirections(cur, cmd, envp, state);
        if (!cur)
        {
            free_partial_cmd(cmd, -1);
            free_redirs(cmd->redirs);
            cmd->redirs = NULL;
            // Stop parsing further commands after syntax error
            return (NULL);
        }
        if (cur && cur->type == TOKEN_WORD)
        {
            cur = parse_arguments(cur, cmd, envp, state, &argc_argv, &argc_final_text);
            if (!cur)
            {
                free_partial_cmd(cmd, -1);
                free_redirs(cmd->redirs);
                cmd->redirs = NULL;
                // Stop parsing further commands after syntax error
                return (NULL);
            }
        }
    }
    return (cur);
}


t_token	*parse_redirections(t_token *cur, t_cmd *cmd, char **envp, t_exec_state *state)
{
	t_redir	*new_redir;
	t_redir	*last;

    if (!cur)
        return (NULL);
    if (!cur->next || cur->next->type != TOKEN_WORD)
    {
        // Detect which redirection type and print appropriate error
        extern int g_exit_code;
        if (cur->type == TOKEN_REDIRECT_OUT)
            ft_putstr_fd("minishell: syntax error near unexpected token `>'\n", 2);
        else if (cur->type == TOKEN_REDIRECT_IN)
            ft_putstr_fd("minishell: syntax error near unexpected token '<'\n", 2);
        else if (cur->type == TOKEN_APPEND)
            ft_putstr_fd("minishell: syntax error near unexpected token '>>'\n", 2);
        else if (cur->type == TOKEN_HEREDOC)
            ft_putstr_fd("minishell: syntax error near unexpected token '<<'\n", 2);
        g_exit_code = 2;
        return (NULL);
    }

	// Create a new redirection node
    // new_redir = create_redir(cur, cmd->argv, NULL); // INCORRECT: passed argv as envp

    new_redir = create_redir(cur, envp, state); // pass provided envp and state
	if (!new_redir)
	{
		// Error creating redirection, but we'll continue parsing
		// This ensures we don't stop the whole parsing process for one failed redirection
		return (cur->next->next);
	}

	// Add the redirection to the command's redirection list
	if (!cmd->redirs)
	{
		cmd->redirs = new_redir;
	}
	else
	{
		last = cmd->redirs;
		while (last->next)
			last = last->next;
		last->next = new_redir;
	}
	
	// Move past the redirection token and its argument
	return (cur->next->next);
}


/* parse_arguments moved to parser_utils.c to allow persistent argc counters */

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