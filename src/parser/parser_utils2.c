/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 21:21:22 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/12 16:35:51 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

t_redir	*create_redir(t_token *cur)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = cur->type;
	redir->quoted = is_quoted(cur->next->value);
	redir->file = remove_quotes(cur->next->value);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->next = NULL;
	return (redir);
}

int	add_argument(t_cmd *cmd, char *value, int *argc)
{
	cmd->argv[*argc] = ft_strdup(value);
	if (!cmd->argv[*argc])
	{
		while (--(*argc) >= 0)
			free(cmd->argv[*argc]);
		free(cmd->argv);
		cmd->argv = NULL;
		return (0);
	}
	(*argc)++;
	return (1);
}

void	print_redirs(t_redir *redir)
{
	while (redir)
	{
		printf("  vaRedir type: %d, file: %s\n", redir->type, redir->file);
		redir = redir->next;
	}
}

void	print_cmd_list(t_cmd *cmd_list)
{
	int	i;

	while (cmd_list)
	{
		printf("Command:\n");
		if (cmd_list->argv)
		{
			i = 0;
			while (cmd_list->argv[i])
			{
				printf("  argv[%d]: %s\n", i, cmd_list->argv[i]);
				i++;
			}
		}
		else
			printf("  argv is NULL\n");
		printf("  Pipe: %d\n", cmd_list->pipe);
		if (cmd_list->redirs)
			print_redirs(cmd_list->redirs);
		else
			printf("  No redirections\n");
		printf("\n");
		cmd_list = cmd_list->next;
	}
}

bool	is_quoted(const char *str)
{
	int	len;

	len = ft_strlen(str);
	if (len >= 2)
	{
		if ((str[0] == '"' && str[len - 1] == '"')
			|| (str[0] == '\'' && str[len - 1] == '\''))
			return (true);
	}
	return (false);
}
