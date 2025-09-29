/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/29 17:13:12 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_interactive_shell(char ***envp_copy, t_exec_state *state)
{
	char	*input;

	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
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

/*int	main(int argc, char **argv, char **envp)
{
	char			**envp_copy;
	t_exec_state	state;

	(void)argc;
	(void)argv;
	envp_copy = new_envp(envp);
	if (!envp_copy)
		return (1);
	update_shlvl(envp_copy);
	state.last_status = 0;
	run_interactive_shell(&envp_copy, &state);
	free_envp(envp_copy);
	return (state.last_status);
}*/

static void	run_non_interactive_shell(char ***envp_copy, 
	t_exec_state *state)
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
		if (state->last_status == 2)
		{
			free(input);
			break ;
		}
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
