/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 18:52:48 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 16:32:53 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
static void	process_unquoted_filename(t_redir *redir)
{
	char		*tmp;
	t_redir		*redir;
	int			res;
	t_fragment	*frag;
	bool		is_quoted;
	char		*built_delim;
	char		*tmp;

	tmp = remove_quotes(redir->file);
	free(redir->file);
	redir->file = tmp;
} */
/*
int	handle_redirections_and_quotes(t_redir *redirs, char **envp,
		t_exec_state *state)
{
	redir = redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			// Decidir si expandir usando los fragments
			frag = redir->fragments;
			is_quoted = false;
			while (frag)
			{
				if (frag->quote_type == QUOTE_SINGLE
					|| frag->quote_type == QUOTE_DOUBLE)
				{
					is_quoted = true;
					break ;
				}
				frag = frag->next;
			}
			redir->quoted = is_quoted;
			// Reconstruir delimiter
			built_delim = build_heredoc_delimiter(redir->file);
			if (!built_delim)
				return (1);
			free(redir->file);
			redir->file = built_delim;
		}
		else
		{
			tmp = remove_quotes(redir->file);
			free(redir->file);
			redir->file = tmp;
		}
		redir = redir->next;
	}
	res = handle_redirections(redirs, envp, state);
	return (res);
}
 */
int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		/* If argv[0] is empty string, treat as command not found (127).
		   Only NULL cmd is a parser-level syntax error which should not
		   reach here. */
		if (!cmd || !cmd->argv)
		{
			ft_putstr_fd("[EXEC_UTIL ERR] minishell: syntax error near unexpected token `|'\n",
				STDERR_FILENO);
			return (2);
		}
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->argv[0] ? cmd->argv[0] : "", STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		return (127);
	}
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		return (127);
	}
	return (execute_execve(exec_path, cmd->argv, envp));
}

char	*str_append(char *base, const char *add)
{
	char	*new;
	size_t	len;

	len = 0;
	if (base)
		len += ft_strlen(base);
	if (add)
		len += ft_strlen(add);
	new = malloc(len + 1);
	if (!new)
		return (NULL);
	new[0] = '\0';
	if (base)
		ft_strlcat(new, base, len + 1);
	if (add)
		ft_strlcat(new, add, len + 1);
	free(base);
	return (new);
}

char	*build_heredoc_delimiter(const char *text)
{
	t_fragment	*fragments;
	char		*delimiter;
	t_token		dummy_tok;

	delimiter = NULL;
	if (!text)
		return (ft_strdup(""));
	fragments = parse_mixed_fragments(text);
	if (!fragments)
		return (ft_strdup(text));
	dummy_tok.fragments = fragments;
	delimiter = concat_final_text(&dummy_tok);
	free_fragments(fragments);
	return (delimiter);
}
