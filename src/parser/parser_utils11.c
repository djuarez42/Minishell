/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils11.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 22:11:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 19:18:10 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	has_expandable_chars(t_fragment *frag)
{
	int	i;

	i = 0;
	while (frag->text[i])
	{
		if (frag->text[i] == '$')
			return (1);
		if (frag->text[i] == '~' && i == 0 && frag->quote_type == QUOTE_NONE)
			return (1);
		i++;
	}
	return (0);
}

int	should_expand_fragment(t_fragment *frag)
{
	if (!frag || !frag->text)
		return (0);
	if (frag->quote_type == QUOTE_SINGLE)
		return (0);
	if (frag->quote_type == QUOTE_DOUBLE || frag->quote_type == QUOTE_NONE)
		return (has_expandable_chars(frag));
	if (frag->quote_type == QUOTE_DOLLAR)
		return (1);
	return (0);
}

void	expand_fragments(t_token *tok, char **envp, t_exec_state *state)
{
	t_fragment	*frag;

	frag = tok->fragments;
	 while (frag)
	 {
	 	if (should_expand_fragment(frag)) {
	 		frag->expanded_text = expand_fragment(frag->text,
	 				frag->quote_type, envp, state);
	 		if (!frag->expanded_text)
	 			frag->expanded_text = ft_strdup("");
	 	} else {
	 		frag->expanded_text = ft_strdup(frag->text);
	 		if (!frag->expanded_text)
	 			frag->expanded_text = ft_strdup("");
	 	}
	 	frag = frag->next;
	 }
}

char	*expand_fragment(const char *text, t_quote_type quote,
			char **envp, t_exec_state *state)
{
	char	*result;
	char	*tmp;

	if (!text)
		return (ft_strdup(""));
	if (quote == QUOTE_SINGLE)
		return (ft_strdup(text));
	if (quote == QUOTE_DOLLAR)
		return (expand_ansi_c_string(text));
	if (text[0] == '~' && quote == QUOTE_NONE)
		return (expand_tilde_bash(text, envp));
	result = expand_command_substitutions(text, envp);
	if (!result)
		return (NULL);
	tmp = expand_variables(result, envp, state, quote);
	free(result);
	return (tmp);
}

int	is_delimiter(const char *line, const char *delimiter)
{
	if (!line)
		return (0);
	if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0
		&& line[ft_strlen(delimiter)] == '\0')
		return (1);
	return (0);
}
