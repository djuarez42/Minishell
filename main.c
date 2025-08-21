/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/21 18:17:46 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "signals.h"

static char	*read_stdin_line(void)
{
	char	buffer[1];
	char	*line;
	ssize_t	read_bytes;
	char	tmp[2];

	line = NULL;
	while (1)
	{
		read_bytes = read(STDIN_FILENO, buffer, 1);
		if (read_bytes <= 0)
			break ;
		if (buffer[0] == '\n')
			break ;
		tmp[0] = buffer[0];
		tmp[1] = '\0';
		line = ft_strjoin_free(line, tmp);
		if (!line)
			return (NULL);
	}
	if (!line && read_bytes <= 0)
		return (NULL);
	if (!line)
		return (ft_strdup(""));
	return (line);
}

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
	if (isatty(STDIN_FILENO))
		signals_setup_interactive(&state);
	while (1)
	{
		if (isatty(STDIN_FILENO))
			input = readline("minishell$ ");
		else
			input = read_stdin_line();
		if (!input)
		{
			if (isatty(STDIN_FILENO))
				ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
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
		//print_token_list(tokens);
		free_token_list(tokens);
		free_cmds(cmds);
		free(input);
	}
	if (isatty(STDIN_FILENO))
		signals_teardown_interactive();
	free_envp(envp_copy);
	return (0);
}
