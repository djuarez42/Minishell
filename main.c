/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 01:20:00 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "signals.h"

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
    size_t n_tests = sizeof(tests)/sizeof(tests[0]);

    for (size_t i = 0; i < n_tests; i++)
    {
        printf("\n=== TEST %zu: %s ===\n", i + 1, tests[i]);

        // 1️⃣ Tokenizamos la entrada
        t_token *raw_tokens = tokenize_input(tests[i]);
        if (!raw_tokens)
        {
            printf("Error tokenizing input\n");
            continue;
        }

        // 2️⃣ Construimos lista final de tokens concatenando fragments
        t_token *final_tokens = build_token_list_from_fragments(raw_tokens);
        if (!final_tokens)
        {
            printf("Error building token list\n");
            free_tokens(raw_tokens);
            continue;
        }

        // 3️⃣ Imprimimos la lista de tokens final
        print_final_token_list(final_tokens);

        // 4️⃣ Parseamos tokens a lista de comandos
        t_cmd *cmd_list = parser_tokens(final_tokens);
        if (!cmd_list)
        {
            printf("Error parsing tokens to commands\n");
            free_tokens(raw_tokens);
            free_tokens(final_tokens);
            continue;
        }

        // 5️⃣ Imprimimos lista de comandos
        print_cmd_list(cmd_list);

        // 6️⃣ Liberamos memoria
        free_tokens(raw_tokens);
        free_tokens(final_tokens);
        free_cmds(cmd_list);
    }

    return 0;
}*/

int main(int argc, char **argv, char **envp)
{
    char        *input;
    t_token     *tokens;
    t_cmd       *cmds;
    char        **envp_copy;
    t_exec_state state;
    t_cmd       *cur;
    int         fail;

    (void)argc;
    (void)argv;

    // Copia dinámica de envp
    envp_copy = new_envp(envp);
    if (!envp_copy)
        return (1);

    state.last_status = 0;

    while (1)
    {
        fail = 0;
        input = readline("minishell$ ");
        if (!input)
            break ; // Ctrl+D

        if (*input)
            add_history(input);

        // 1. Lexer: tokens limpios listos para parser
        tokens = tokenize_input(input);
        if (!tokens)
        {
            free(input);
            continue ;
        }

        // 2. Parser: construye lista de comandos
        cmds = parser_tokens(tokens);
        if (!cmds)
        {
            free_tokens(tokens);
            free(input);
            continue ;
        }

        // 3. Expansión de variables / sustituciones
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

        // 4. Ejecutar
        if (!fail)
            executor(cmds, &envp_copy, &state);

        // 5. Liberar memoria
        free_tokens(tokens);
        free_cmds(cmds);
        free(input);

        if (fail)
        {
            free_envp(envp_copy);
            return (1);
        }
    }

    free_envp(envp_copy);
    return (state.last_status);
}