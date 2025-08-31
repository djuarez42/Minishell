/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/31 23:00:45 by djuarez          ###   ########.fr       */
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

/*int main(void)
{
    const char *tests[] = {
        "echo hello",
        "ls -l | grep 'test file'",
        "echo \"double quoted text\" end",
        "mix\"double\\\"escaped\"and'singles'",
        "echo \"hello world\" 'and good bye'",
        "\"\"ec\"\"ho\"\" hola\"\"mundo\"\"",
        "\"\"echo\"\""
    };

    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++)
    {
        printf("\n=== Test %d: %s ===\n", i + 1, tests[i]);

        t_token *raw_tokens = tokenize_input(tests[i]);
        if (!raw_tokens)
        {
            printf("Error: Invalid input (unmatched quotes)\n");
            continue;
        }

        printf("--- Raw tokens from tokenize_input ---\n");
        print_tokens(raw_tokens);

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

        free_tokens(raw_tokens);
        free_tokens(tokens);
    }

    return 0;
}*/


/*t_fragment *make_test_fragments(void)
{
    t_fragment *f1 = malloc(sizeof(t_fragment));
    f1->text = strdup("ec");
    f1->quote_type = 0;
    f1->has_space_after = 0;

    t_fragment *f2 = malloc(sizeof(t_fragment));
    f2->text = strdup("ho");
    f2->quote_type = 0;
    f2->has_space_after = 1; // <- simula espacio después de "ho"

    t_fragment *f3 = malloc(sizeof(t_fragment));
    f3->text = strdup("hola");
    f3->quote_type = 0;
    f3->has_space_after = 0;

    t_fragment *f4 = malloc(sizeof(t_fragment));
    f4->text = strdup("mundo");
    f4->quote_type = 0;
    f4->has_space_after = 0;

    f1->next = f2;
    f2->next = f3;
    f3->next = f4;
    f4->next = NULL;

    return f1;
}

int main(void)
{
    printf("=== Test: \"\"ec\"\"ho\"\" hola\"\"mundo\"\" ===\n");

    // 1. Crear lista de fragmentos simulada
    t_token raw;
    raw.type = 0;
    raw.has_space_before = 0;
    raw.fragments = make_test_fragments();
    raw.final_text = NULL;
    raw.next = NULL;

    // 2. Construir tokens finales
    t_token *tokens = build_token_list_from_fragments(&raw);

    // 3. Imprimir tokens para depuración
    print_token_list_from_fragments(tokens);
    print_tokens(tokens);

    return 0;
}*/


/*int main(void)
{
    const char *tests[] = {
        "echo hello",
        "ls -l | grep 'test file'",
        "echo \"double quoted text\" end",
        "mix\"double\\\"escaped\"and'singles'",
        "echo \"hello world\" 'and good bye'",
        "\"\"ec\"\"ho\"\" hola\"\"mundo\"\"",
        "\"\"echo\"\""
    };

    int n = sizeof(tests) / sizeof(tests[0]);
    int i = 0;

    while (i < n)
    {
        printf("\n=== Test %d: %s ===\n", i + 1, tests[i]);

        t_token *raw_tokens = tokenize_input(tests[i]);
        if (!raw_tokens)
        {
            printf("Error: Invalid input (unmatched quotes)\n");
            i++;
            continue;
        }

        t_token *final_tokens = build_token_list_from_fragments(raw_tokens);
        if (!final_tokens)
        {
            printf("Error: Failed to build token list\n");
            free_tokens(raw_tokens);
            i++;
            continue;
        }

        print_final_token_list(final_tokens);

        free_tokens(raw_tokens);
        free_tokens(final_tokens);
        i++;
    }

    return 0;
}*/

/*int main(void)
{
    const char *tests[] = {
        "echo hello",
        "ls -l | grep 'test file'",
        "echo \"double quoted text\" end",
        "mix\"double\\\"escaped\"and'singles'",
        "echo \"hello world\" 'and good bye'",
        "\"\"ec\"\"ho\"\" hola\"\"mundo\"\"",
        "\"\"echo\"\""
    };

    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++)
    {
        printf("\n=== Test %d: %s ===\n", i + 1, tests[i]);

        // Tokenización inicial
        t_token *raw_tokens = tokenize_input(tests[i]);
        if (!raw_tokens)
        {
            printf("Error: Invalid input (unmatched quotes)\n");
            continue;
        }

        // Construcción de la lista final de tokens
        t_token *final_tokens = build_token_list_from_fragments(raw_tokens);
        if (!final_tokens)
        {
            printf("Error: Failed to build final token list\n");
            free_tokens(raw_tokens);
            continue;
        }

        // Imprimir lista final de tokens
        print_final_token_list(final_tokens);

        // Liberar memoria
        free_tokens(raw_tokens);
        free_tokens(final_tokens);
    }

    return 0;
}*/

int main(void)
{
    const char *tests[] = {
        "echo hello",
        "ls -l | grep 'test file'",
        "echo \"double quoted text\" end",
        "mix\"double\\\"escaped\"and'singles'",
        "echo \"hello world\" 'and good bye'",
        "\"\"ec\"\"ho\"\" hola\"\"mundo\"\"",
        "\"\"echo\"\""
    };

    int n_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n_tests; i++)
    {
        printf("=== Test %d: %s ===\n", i + 1, tests[i]);

        // 1️⃣ Tokenizar input
        t_token *raw_tokens = tokenize_input((char *)tests[i]); 
        // tokenize_input debe devolver la lista de t_token con fragments

        // 2️⃣ Construir lista final de tokens
        t_token *final_tokens = build_token_list_from_fragments(raw_tokens);

        // 3️⃣ Imprimir lista final
        print_final_token_list(final_tokens);

        // 4️⃣ Liberar memoria (tus funciones)
        free_tokens(raw_tokens);
        free_tokens(final_tokens);
    }

    return 0;
}
