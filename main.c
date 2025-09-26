/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/26 18:18:36 by ekakhmad         ###   ########.fr       */
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

static void	process_input(char *input, char ***envp_copy, t_exec_state *state)
{
	t_token	*tokens;
	t_cmd	*cmds;
	t_token	*tmp;
	t_cmd	*tmp;
	bool	has_command;
	int		err;

	tokens = tokenize_input(input);
	/* if (getenv("MINISHELL_DEBUG_TOKENS") && tokens)
	{
		tmp = tokens;
		while (tmp)
		{
			fprintf(stderr,
				"[MAIN TOK] type=%d text='%s' has_space_before=%d\n", tmp->type,
				tmp->final_text ? tmp->final_text : "(null)",
				tmp->has_space_before);
			tmp = tmp->next;
		}
	} */
	if (!tokens)
	{
		/* If tokenize_input failed due to unmatched quotes, set exit status 2
			so non-interactive mode will stop and exit with bash-compatible code. */
		if (check_unmatched_quotes(input))
			state->last_status = 2;
		return ;
	}
	cmds = parser_tokens(tokens, *envp_copy, state);
	if (cmds)
	{
		// Check if there is at least one argv (command) in the cmds list
		tmp = cmds;
		has_command = false;
		while (tmp)
		{
			if (tmp->argv && tmp->argv[0] && tmp->argv[0][0] != '\0')
			{
				has_command = true;
				break ;
			}
			tmp = tmp->next;
		}
		if (has_command)
		{
			executor(cmds, envp_copy, state);
		}
		else
		{
			err = 1;
			/* If there are no argv (empty command like ""), bash treats this
				as an attempt to execute an empty command and returns 127 with
				a "command not found" message. Emulate that behavior. */
			if (cmds->argv && cmds->argv[0] && cmds->argv[0][0] == '\0')
			{
				errno = ENOENT;
				print_error(cmds->argv[0], "command not found");
				state->last_status = 127;
			}
			else
			{
				/* Protect against NULL redirections list
					- parser may produce cmds
					without argv and without redirs (e.g. empty input). Only call
					the redirection handler when a redirection and filename exist. */
				if (cmds->redirs && cmds->redirs->file)
					handle_redirections_append(cmds->redirs->file, &err, state);
				/* Error and exit code handled by redirection handler when called.
					Otherwise there is nothing to do for pure empty commands. */
			}
		}
		free_cmds(cmds);
	}
	free_token_list(tokens);
}

static void	run_interactive_shell(char ***envp_copy, t_exec_state *state)
{
	char	*input;
	char	*full;
	char	*cont;
	int		single_open;
	int		double_open;
	int		i;
	char	*tmp1;
	char	*tmp2;
	char	*tmp1;

	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			break ;
		/* Build full input and support PS2-style continuation when quotes
			are left open by reading additional lines with the "> " prompt. */
		full = ft_strdup(input);
		free(input);
		if (!full)
			break ;
		while (check_unmatched_quotes(full))
		{
			cont = readline("> ");
			if (!cont)
			{
				/* EOF during interactive continuation: report matching message
					then a general unmatched-quotes syntax error (interactive). */
				single_open = 0;
				double_open = 0;
				i = 0;
				while (full[i])
				{
					if (full[i] == '\'' && double_open == 0)
						single_open = !single_open;
					else if (full[i] == '"' && single_open == 0)
						double_open = !double_open;
					i++;
				}
				if (single_open)
					print_error(NULL,
						"unexpected EOF while looking for matching `''");
				else if (double_open)
					print_error(NULL,
						"unexpected EOF while looking for matching `\"");
				print_error(NULL, "syntax error: unmatched quotes");
				state->last_status = 2;
				free(full);
				full = NULL;
				break ;
			}
			if (cont && *cont)
			{
				tmp1 = ft_strjoin(full, "\n");
				tmp2 = ft_strjoin(tmp1, cont);
				free(tmp1);
				free(full);
				free(cont);
				full = tmp2;
				if (!full)
					break ;
			}
			else
			{
				/* Append the newline even if continuation is empty to preserve
					same layout as bash when multiline commands are entered. */
				tmp1 = ft_strjoin(full, "\n");
				free(full);
				full = tmp1;
				free(cont);
				if (!full)
					break ;
			}
		}
		if (!full)
		{
			if (state->last_status == 2)
				break ;
			continue ;
		}
		if (*full)
			add_history(full);
		process_input(full, envp_copy, state);
		free(full);
		if (state->last_status == 2)
			break ;
	}
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
static void	run_non_interactive_shell(char ***envp_copy, t_exec_state *state)
{
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
		while (check_unmatched_quotes(input))
		{
			read_len = getline(&line, &len, stdin);
			if (read_len == -1)
			{
				{
					single_open = 0;
					double_open = 0;
					i = 0;
					while (input[i])
					{
						if (input[i] == '\'' && double_open == 0)
							single_open = !single_open;
						else if (input[i] == '"' && single_open == 0)
							double_open = !double_open;
						i++;
					}
					if (single_open)
						print_error(NULL,
							"unexpected EOF while looking for matching `''");
					else if (double_open)
						print_error(NULL,
							"unexpected EOF while looking for matching `\"'");
					if (!isatty(STDIN_FILENO))
						print_error(NULL,
							"syntax error: unexpected end of file");
					else
						print_error(NULL, "syntax error: unmatched quotes");
				}
				state->last_status = 2;
				free(input);
				break ;
			}
			if (read_len > 0 && line[read_len - 1] == '\n')
				line[read_len - 1] = '\0';
			tmp1 = ft_strjoin(input, "\n");
			tmp2 = ft_strjoin(tmp1, line);
			free(tmp1);
			free(input);
			input = tmp2;
			if (!input)
				break ;
		}
		if (state->last_status == 2)
		{
			break ;
		}
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
