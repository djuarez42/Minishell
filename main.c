/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/10/03 16:59:03 by ekakhmad         ###   ########.fr       */
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

static void	process_non_interactive_line(char *line, char ***envp_copy,
		t_exec_state *state)
{
	char	*input;
	char	**delimiters;
	char	*heredoc_bodies;

	input = ft_strdup(line);
	if (!input)
		return ;
	delimiters = find_heredoc_delimiters(input);
	if (delimiters)
	{
		heredoc_bodies = collect_heredoc_bodies_into_buffer(delimiters);
		if (heredoc_bodies)
		{
			heredoc_buffer_init(state, heredoc_bodies);
			free(heredoc_bodies);
		}
		free_split(delimiters);
	}
	process_input(input, envp_copy, state);
	heredoc_buffer_free(state);
	free(input);
}

static void	run_non_interactive_shell(char ***envp_copy,
		t_exec_state *state)
{
	char	*line;

	line = get_next_line(STDIN_FILENO);
	while (line)
	{
		process_non_interactive_line(line, envp_copy, state);
		free(line);
		if (state->last_status == 2)
			break ;
		line = get_next_line(STDIN_FILENO);
	}
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
	state.heredoc_buffer = NULL;
	state.heredoc_position = NULL;
	is_interactive = isatty(STDIN_FILENO);
	if (is_interactive)
		run_interactive_shell(&envp_copy, &state);
	else
		run_non_interactive_shell(&envp_copy, &state);
	free_envp(envp_copy);
	return (state.last_status);
}
