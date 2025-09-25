/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils6.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 20:51:04 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/25 17:02:30 by ekakhmad         ###   ########.fr       */
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

char	*concat_final_text(t_token *tok)
{
	t_fragment	*frag;
	char		*final_text;
	char		*tmp;
	char		*new_result;

	frag = tok->fragments;
	final_text = ft_strdup("");
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
	return (final_text);
}

static t_cmd	*alloc_cmd_struct(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = malloc(sizeof(char *) * MAX_ARGS);
	cmd->argv_quote = malloc(sizeof(t_quote_type) * MAX_ARGS);
	cmd->argv_final_text = malloc(sizeof(char *) * MAX_ARGS);
	cmd->argv_first_word = malloc(sizeof(bool) * MAX_ARGS);
	cmd->freed_by_parser = false;
	if (!cmd->argv || !cmd->argv_quote
		|| !cmd->argv_final_text || !cmd->argv_first_word)
	{
		free(cmd->argv);
		free(cmd->argv_quote);
		free(cmd->argv_final_text);
		free(cmd->argv_first_word);
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

static void	init_cmd_arrays(t_cmd *cmd)
{
	int	i;

	i = 0;
	while (i < MAX_ARGS)
	{
		cmd->argv[i] = NULL;
		cmd->argv_quote[i] = QUOTE_NONE;
		cmd->argv_final_text[i] = NULL;
		cmd->argv_first_word[i] = false;
		i++;
	}
	cmd->redirs = NULL;
	cmd->pipe = 0;
	cmd->next = NULL;
}

t_cmd	*create_cmd_node(t_token **cur, char **envp, t_exec_state *state)
{
	t_cmd	*cmd;

	cmd = alloc_cmd_struct();
	if (!cmd)
		return (NULL);
	init_cmd_arrays(cmd);
	*cur = parse_cmd_block(*cur, cmd, envp, state);
	if (!*cur)
	{
		if (!cmd->freed_by_parser)
			free_cmd_arrays(cmd);
		free(cmd);
		return (NULL);
	}
	if ((*cur)->type == TOKEN_PIPE)
	{
		cmd->pipe = 1;
		*cur = (*cur)->next;
	}
	return (cmd);
}
