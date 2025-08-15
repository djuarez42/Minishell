/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/15 18:04:13 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

int	main(int argc, char **argv, char **envp)
{
	t_exec_state	state;
	char			*input;
	t_token			*tokens;
	t_cmd			*cmds;
	char			**envp_copy;
	t_cmd			*cur;

	(void)argc;
	(void)argv;
	envp_copy = new_envp(envp);
	if (!envp_copy)
		return (1);
	state = (t_exec_state){0};
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			break ;
		if (*input)
			add_history(input);
		tokens = tokenize_input(input);
		if (!tokens)
		{
			free(input);
			continue ;
		}
		print_token_list(tokens);
		cmds = parser_tokens(tokens);
		cur = cmds;
		while (cur)
		{
			if (expand_cmd_inplace(cur, envp_copy, &state) == -1)
				exit (1);
			cur = cur->next;
		}
		if (cmds)
			executor(cmds, &envp_copy, &state);
		print_token_list(tokens);
		free_token_list(tokens);
		free_cmds(cmds);
		free(input);
	}
	free_envp(envp_copy);
	return (0);
}
