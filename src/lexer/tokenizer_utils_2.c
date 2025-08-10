/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 18:46:30 by djuarez           #+#    #+#             */
/*   Updated: 2025/07/26 17:48:57 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

t_token_type	determine_token_type(char *str)
{
	if (ft_strncmp(str, "<<", 2) == 0)
		return (TOKEN_HEREDOC);
	else if (ft_strncmp(str, ">>", 2) == 0)
		return (TOKEN_APPEND);
	else if (ft_strncmp(str, "<", 1) == 0)
		return (TOKEN_REDIRECT_IN);
	else if (ft_strncmp(str, ">", 1) == 0)
		return (TOKEN_REDIRECT_OUT);
	else if (ft_strncmp(str, "|", 1) == 0)
		return (TOKEN_PIPE);
	else
		return (TOKEN_WORD);
}

t_token	*build_token_list(char **tokens)
{
	t_token	*head;
	t_token	*cur;
	t_token	*new;
	int		i;

	head = NULL;
	cur = NULL;
	i = 0;
	while (tokens[i])
	{
		new = malloc(sizeof(t_token));
		if (!new)
			return (NULL);
		new->value = ft_strdup(tokens[i]);
		new->type = determine_token_type(tokens[i]);
		new->next = NULL;
		if (!head)
			head = new;
		else
			cur->next = new;
		cur = new;
		i++;
	}
	return (append_token_eof(head, cur));
}

void	print_token_list(t_token *token)
{
	while (token)
	{
		printf("Type: %-15s Value: \"%s\"\n",
			token_type_str(token->type), token->value);
		token = token->next;
	}
}

void	free_token_list(t_token *head)
{
	t_token	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->value);
		free(tmp);
	}
}

t_token	*append_token_eof(t_token *head, t_token *cur)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->value = NULL;
	new->type = TOKEN_EOF;
	new->next = NULL;
	if (!head)
		return (new);
	cur->next = (new);
	return (head);
}
