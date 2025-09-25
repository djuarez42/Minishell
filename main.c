/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/25 22:06:50 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
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
			int err = 1;
			/* If there are no argv (empty command like ""), bash treats this
			   as an attempt to execute an empty command and returns 127 with
			   a "command not found" message. Emulate that behavior. */
			if (cmds->argv && cmds->argv[0] && cmds->argv[0][0] == '\0')
			{
				errno = ENOENT;
				if (!isatty(STDIN_FILENO))
				{
					/* Non-interactive: include line number prefix similar to bash */
					ft_putstr_fd("minishell: ", STDERR_FILENO);
					ft_putstr_fd("line 1: ", STDERR_FILENO);
					ft_putstr_fd(": ", STDERR_FILENO);
					ft_putendl_fd("command not found", STDERR_FILENO);
				}
				else
				{
					print_execve_error(cmds->argv[0]);
				}
				state->last_status = 127;
			}
			else
			{
				/* Protect against NULL redirections list - parser may produce cmds
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
	char    *input;
	char    *full;

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
			char *cont = readline("> ");
			if (!cont)
			{
				/* EOF during interactive continuation: report matching message
				   then a general unmatched-quotes syntax error (interactive). */
				int single_open = 0;
				int double_open = 0;
				int i = 0;
				while (full[i])
				{
					if (full[i] == '\'' && double_open == 0)
						single_open = !single_open;
					else if (full[i] == '"' && single_open == 0)
						double_open = !double_open;
					i++;
				}
				if (single_open)
					print_error(NULL, "unexpected EOF while looking for matching `''");
				else if (double_open)
					print_error(NULL, "unexpected EOF while looking for matching `\"'");
				print_error(NULL, "syntax error: unmatched quotes");
				state->last_status = 2;
				free(full);
				full = NULL;
				break ;
			}
			if (cont && *cont)
			{
				char *tmp1 = ft_strjoin(full, "\n");
				char *tmp2 = ft_strjoin(tmp1, cont);
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
				char *tmp1 = ft_strjoin(full, "\n");
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
		/* Build full input, concatenating subsequent lines if quotes are
		   unmatched so multi-line commands are supported (like bash). */
		input = ft_strdup(line);
		if (!input)
			break ;
		while (check_unmatched_quotes(input))
		{
			/* Need more lines to close quotes */
			read_len = getline(&line, &len, stdin);
			if (read_len == -1)
			{
				/* Print an "unexpected EOF" line for the specific open quote then
				   a general syntax error line so output matches bash's two-line
				   reporting when EOF occurs while a quote is left open. */
				{
					int single_open = 0;
					int double_open = 0;
					int i = 0;
					while (input[i])
					{
						if (input[i] == '\'' && double_open == 0)
							single_open = !single_open;
						else if (input[i] == '"' && single_open == 0)
							double_open = !double_open;
						i++;
					}
					if (single_open)
						print_error(NULL, "unexpected EOF while looking for matching `''");
					else if (double_open)
						print_error(NULL, "unexpected EOF while looking for matching `\"'");
					if (!isatty(STDIN_FILENO))
						print_error(NULL, "syntax error: unexpected end of file");
					else
						print_error(NULL, "syntax error: unmatched quotes");
				}
				state->last_status = 2;
				free(input);
				break ;
			}
			if (read_len > 0 && line[read_len - 1] == '\n')
				line[read_len - 1] = '\0';
			/* append '\n' and the next line to input */
			char *tmp1 = ft_strjoin(input, "\n");
			char *tmp2 = ft_strjoin(tmp1, line);
			free(tmp1);
			free(input);
			input = tmp2;
			if (!input)
				break ;
		}
		if (state->last_status == 2)
		{
			/* syntax error at EOF, stop processing */
			break ;
		}
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
