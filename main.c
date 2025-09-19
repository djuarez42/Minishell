/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/19 17:26:49 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	process_input(char *input, char ***envp_copy, t_exec_state *state)
{
	t_token	*tokens;
	t_cmd	*cmds;

	tokens = tokenize_input(input);
	if (!tokens)
		return ;
	cmds = parser_tokens(tokens, *envp_copy, state);
	if (cmds)
	{
		executor(cmds, envp_copy, state);
		free_cmds(cmds);
	}
	free_token_list(tokens);
}

static void	run_interactive_shell(char ***envp_copy, t_exec_state *state)
{
	char	*input;

	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			break ;
		if (*input)
			add_history(input);
		process_input(input, envp_copy, state);
		free(input);
	}
}

static void	run_non_interactive_shell(char ***envp_copy, t_exec_state *state)
{
	char	*line;
	char	*input;
	size_t	len;
	ssize_t	read_len;

	line = NULL;
	len = 0;
	read_len = getline(&line, &len, stdin);
	while (read_len != -1)
	{
		if (read_len > 0 && line[read_len - 1] == '\n')
			line[read_len - 1] = '\0';
		input = ft_strdup(line);
		if (!input)
			break ;
		process_input(input, envp_copy, state);
		free(input);
		read_len = getline(&line, &len, stdin);
	}
	if (line)
		free(line);
}

int	main(int argc, char **argv, char **envp)
{
	char			**envp_copy;
	t_exec_state	state;
	int				is_interactive;

	(void)argc;
	(void)argv;
	envp_copy = new_envp(envp);
	if (!envp_copy)
		return (1);
	state.last_status = 0;
	is_interactive = isatty(STDIN_FILENO);
	if (is_interactive)
		run_interactive_shell(&envp_copy, &state);
	else
		run_non_interactive_shell(&envp_copy, &state);
	free_envp(envp_copy);
	return (state.last_status);
}
