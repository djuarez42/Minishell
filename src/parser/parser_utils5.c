/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils5.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 20:47:45 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/23 21:22:15 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*parse_redirections_loop(t_token *cur, t_cmd *cmd)
{
	while (cur && (cur->type == TOKEN_REDIRECT_OUT
			|| cur->type == TOKEN_REDIRECT_IN || cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC))
	{
		cur = parse_redirections(cur, cmd);
		if (!cur)
		{
			free_cmd_arrays(cmd);
			cmd->freed_by_parser = true;
			return (NULL);
		}
	}
	return (cur);
}

t_token	*parse_cmd_block(t_token *cur, t_cmd *cmd, char **envp,
		t_exec_state *state)
{
	t_proc_ctx	ctx;
	int			argc_argv_local;
	int			argc_final_text_local;

	argc_argv_local = 0;
	argc_final_text_local = 0;
	ctx.cmd = cmd;
	ctx.argc_argv = &argc_argv_local;
	ctx.argc_final_text = &argc_final_text_local;
	ctx.envp = envp;
	ctx.state = state;
	while (1)
	{
		cur = parse_arguments_ctx(cur, &ctx);
		if (!cur)
			return (NULL);
		cur = parse_redirections_loop(cur, cmd);
		if (!cur)
			return (NULL);
		if (!cur || cur->type != TOKEN_WORD)
			break ;
	}
	return (cur);
}

void	calc_total_len(t_fragment *frag, t_word_builder *wb)
{
	wb->total_len = 0;
	while (frag)
	{
		if (frag->expanded_text)
			wb->total_len += ft_strlen(frag->expanded_text);
		frag = frag->next;
	}
}

void	copy_fragment_to_buffer(t_fragment *frag, t_word_builder *wb)
{
	size_t	i;
	size_t	flen;
	int		frag_splittable;

	flen = ft_strlen(frag->expanded_text);
	frag_splittable = 0;
	if (frag->quote_type == QUOTE_NONE && frag->text && frag->text[0] == '$')
		frag_splittable = 1;
	i = 0;
	while (i < flen)
	{
		wb->buf[wb->len] = frag->expanded_text[i];
		if (frag_splittable)
			wb->splittable[wb->len] = 1;
		else
			wb->splittable[wb->len] = 0;
		wb->len++;
		i++;
	}
}

int	validate_redirection(t_token *cur)
{
	if (getenv("MINISHELL_DEBUG_TOKENS") && cur)
	{
		fprintf(stderr, "[VALIDATE_RED] cur_type=%d next_type=%d\n",
			cur->type, cur->next ? cur->next->type : -1);
	}
	if (!cur->next || cur->next->type != TOKEN_WORD)
	{
		ft_putstr_fd("[PARSER_UTIL ERR] minishell: syntax error near unexpected token `", 2);
		if (cur->type == TOKEN_HEREDOC)
			ft_putstr_fd("<<", 2);
		else if (cur->type == TOKEN_REDIRECT_OUT)
			ft_putstr_fd(">", 2);
		else if (cur->type == TOKEN_APPEND)
			ft_putstr_fd(">>", 2);
		else if (cur->type == TOKEN_REDIRECT_IN)
			ft_putstr_fd("<", 2);
		ft_putendl_fd("'", 2);
		return (0);
	}
	return (1);
}
