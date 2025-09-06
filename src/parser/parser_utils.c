/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:05:32 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/06 18:28:51 by ekakhmad         ###   ########.fr       */
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

int	process_token(t_cmd *cmd, t_token *cur, int *argc,
			char **envp, t_exec_state *state)
{
	t_fragment	*frag;
	char		*arg;
	char		*expanded;
	char		*tmp;

	if (*argc >= MAX_ARGS - 1)
		return (0);

	arg = ft_strdup("");
	if (!arg)
		return (0);

	frag = cur->fragments;
	while (frag)
	{
		expanded = expand_fragment(frag->text, frag->quote_type, envp, state);
		if (!expanded)
		{
			free(arg);
			return (0);
		}
		tmp = str_append(arg, expanded);
		free(expanded);
		arg = tmp;
		if (!arg)
			return (0);
		frag = frag->next;
	}

	cmd->argv[*argc] = arg;
	cmd->argv_quote[*argc] = cur->fragments
		? cur->fragments->quote_type : QUOTE_NONE;
	(*argc)++;
	return (1);
}



