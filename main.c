/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 19:12:43 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1); // keep for signal, not error
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (signo == SIGQUIT)
		(void)signo;
}



/* int	main(int argc, char **argv, char **envp)
{
	char			**envp_copy;
	t_exec_state	state;
	char			*line;
	char			*input;
	size_t			len;
	ssize_t			read_len;
	int				single_open;
	int				double_open;
	int				i;
	char			*tmp1;
	char			*tmp2;

	(void)argc;
	(void)argv;
	envp_copy = new_envp(envp);
	if (!envp_copy)
		return (1);
	state.last_status = 0;
	run_interactive_shell(&envp_copy, &state);
	free_envp(envp_copy);
	return (state.last_status);
} */
static void	process_input(char *input, char ***envp_copy, t_exec_state *state)
{
	t_token	*tokens;
	t_cmd	*cmds;

	tokens = tokenize_input(input);
	if (getenv("MINISHELL_DEBUG_TOKENS") && tokens)
	{
		t_token *tmp = tokens;
		while (tmp)
		{
			fprintf(stderr, "[MAIN TOK] type=%d text='%s' has_space_before=%d\n",
				tmp->type, tmp->final_text ? tmp->final_text : "(null)",
				tmp->has_space_before);
			tmp = tmp->next;
		}
	}
	if (!tokens)
		return ;
	cmds = parser_tokens(tokens, *envp_copy, state);
    if (cmds)
    {
        // Check if there is at least one argv (command) in the cmds list
        t_cmd *tmp = cmds;
        bool has_command = false;
        while (tmp)
        {
            if (tmp->argv && tmp->argv[0] && tmp->argv[0][0] != '\0') {
                has_command = true;
                break;
            }
            tmp = tmp->next;
        }
        if (has_command) {
            executor(cmds, envp_copy, state);
        } else {
            // No actual command in the line. If there are redirections-only,
            // apply them; otherwise do nothing (bash keeps last status).
            if (cmds->redirs)
                (void)handle_redirections(cmds->redirs, *envp_copy, state);
        }
        free_cmds(cmds);
    }
	free_token_list(tokens);
}

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
		/* if parser reported a syntax error, stop processing further lines so
		   the shell exits with the syntax error status (bash returns 2) */
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
