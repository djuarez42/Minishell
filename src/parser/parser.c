/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/03 20:12:38 by djuarez          ###   ########.fr       */
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
    int argc;

    argc = 0;
    if (!init_cmd_args(cmd))
        return (NULL);

    while (cur && cur->type != TOKEN_PIPE && cur->type != TOKEN_EOF)
    {
        if (cur->type == TOKEN_WORD)
        {
            if (!process_token(cmd, cur, &argc, envp, state))
            {
                free_partial_cmd(cmd, argc);
                return (NULL);
            }
        }
        else if (cur->type == TOKEN_REDIRECT_OUT
            || cur->type == TOKEN_REDIRECT_IN
            || cur->type == TOKEN_APPEND
            || cur->type == TOKEN_HEREDOC)
        {
            break;
        }
        cur = cur->next;
    }

    cmd->argv[argc] = NULL;
    cmd->argv_quote[argc] = QUOTE_NONE;
    return (cur);
}


char	*expand_fragment(const char *text, t_quote_type quote,
			char **envp, t_exec_state *state)
{
	char	*expanded;

	if (!text)
		return (ft_strdup(""));

	// Caso 1: comillas simples → literal
	if (quote == QUOTE_SINGLE)
		return (ft_strdup(text));

	// Caso 2: tilde expansion (~ al inicio del fragmento)
	if (text[0] == '~' && (quote == QUOTE_NONE || quote == QUOTE_DOUBLE))
	{
		char *home = getenv("HOME");
		if (home)
		{
			if (text[1] == '\0') // solo "~"
				return (ft_strdup(home));
			else
			{
				// "~" seguido de path
				char *suffix = ft_strdup(text + 1);
				if (!suffix)
					return (NULL);
				char *result = str_append(ft_strdup(home), suffix);
				free(suffix);
				return (result);
			}
		}
		// si no hay $HOME definido, dejar "~" literal
	}

	// Caso 3: variables ($VAR, $?)
	expanded = expand_variables(text, envp, state, quote);
	if (!expanded)
		return (NULL);

	return (expanded);
}

