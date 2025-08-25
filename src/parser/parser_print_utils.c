/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_print_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 18:44:25 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/21 18:53:48 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
