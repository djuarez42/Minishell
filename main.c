/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 14:21:51 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "signals.h"

/*static char	*read_stdin_line(void)
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

int main(int argc, char **argv, char **envp)
{
	t_exec_state state;
	char *input;
	t_token *tokens;
	t_cmd *cmds;
	char **envp_copy;
	t_cmd *cur;
	int fail;

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
		fail = 0;
		if (isatty(STDIN_FILENO))
			input = readline("minishell$ ");
		else
			input = read_stdin_line();

		if (!input)
		{
			if (isatty(STDIN_FILENO))
				ft_putendl_fd("exit", STDOUT_FILENO);
			break;
		}

		if (*input)
			add_history(input);

		tokens = tokenize_input(input);
		if (!tokens)
		{
			free(input);
			continue;
		}

		cmds = parser_tokens(tokens);
		if (!cmds)
		{
			free_token_list(tokens);
			free(input);
			continue;
		}

		cur = cmds;
		while (cur)
		{
			if (expand_cmd_inplace(cur, envp_copy, &state) == -1)
			{
				fail = 1;
				break;
			}
			cur = cur->next;
		}

		if (!fail)
			executor(cmds, &envp_copy, &state);

		// Liberamos todo lo que se creó en esta iteración
		free_token_list(tokens);
		free_cmds(cmds);
		free(input);

		if (fail)
		{
			free_envp(envp_copy);
			return (1);
		}
	}

	if (isatty(STDIN_FILENO))
		signals_teardown_interactive();

	free_envp(envp_copy);
	return (0);
}*/

/*int	main(int argc, char **argv, char **envp)
{
	char			*input;
	t_token			*tokens;
	t_cmd			*cmds;
	char			**envp_copy;
	t_exec_state	state;
	t_cmd			*cur;
	int				fail;

	(void)argc;
	(void)argv;
	envp_copy = new_envp(envp);
	if (!envp_copy)
		return (1);
	state.last_status = 0;
	while (1)
	{
		fail = 0;
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
		cmds = parser_tokens(tokens);
		if (!cmds)
		{
			free_token_list(tokens);
			free(input);
			continue ;
		}
		cur = cmds;
		while (cur)
		{
			if (expand_cmd_inplace(cur, envp_copy, &state) == -1)
			{
				fail = 1;
				break ;
			}
			cur = cur->next;
		}
		if (!fail)
			executor(cmds, &envp_copy, &state);
		print_token_list(tokens);
		free_token_list(tokens);
		free_cmds(cmds);
		free(input);
		if (fail)
		{
			free_envp(envp_copy);
			return (1);
		}
	}
	free_envp(envp_copy);
	return (0);
}*/

int main(void)
{
    const char *tests[] = {
        "echo hello",
        "ls -l | grep 'test file'",
        "echo \"double quoted text\" end",
        "mix\"double\\\"escaped\"and'singles'",
        "echo \"hello world\" 'and good bye'",
        "\"\"ec\"\"ho\"\" hola\"\"mundo",
        "\"\"echo\"\""
    };

    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++)
    {
        printf("\n=== Test %d: %s ===\n", i + 1, tests[i]);

        // 1️⃣ Lexer: obtener tokens con fragments
        t_token *raw_tokens = tokenize_input(tests[i]);
        if (!raw_tokens)
        {
            printf("Error: Invalid input (unmatched quotes)\n");
            continue;
        }

        printf("--- Raw tokens from tokenize_input ---\n");
        print_tokens(raw_tokens);

        // 2️⃣ Construir lista de tokens estilo Bash
        t_token *tokens = build_token_list_from_fragments(raw_tokens);
        if (!tokens)
        {
            printf("Error: Failed to build token list\n");
            free_tokens(raw_tokens);
            continue;
        }

        printf("--- Tokens after build_token_list_from_fragments ---\n");
		print_token_list_from_fragments(tokens);
        print_tokens(tokens);

        // 3️⃣ Liberar memoria
        free_tokens(raw_tokens);
        free_tokens(tokens);
    }

    return 0;
}

