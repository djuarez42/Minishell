/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 22:04:11 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "signals.h"
#include <unistd.h> // for isatty()

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
            free_token_list(raw_tokens);
            continue;
        }

        // 3️⃣ Parseamos tokens a lista de comandos
        t_cmd *cmd_list = parser_tokens(final_tokens);
        if (!cmd_list)
        {
            printf("Error parsing tokens to commands\n");
            free_token_list(raw_tokens);
            free_token_list(final_tokens);
            continue;
        }

        // 5️⃣ Imprimimos lista de comandos
        print_cmd_list(cmd_list);

        // 6️⃣ Liberamos memoria
    free_token_list(raw_tokens);
    free_token_list(final_tokens);
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
    int         is_interactive;

    // Check for -c option (execute command and exit)
    if (argc >= 3 && ft_strncmp(argv[1], "-c", 3) == 0)
    {
        // Running in non-interactive mode with -c option
        envp_copy = new_envp(envp);
        if (!envp_copy)
            return (1);
            
        state.last_status = 0;
        
        // Use the command provided as argument
        input = ft_strdup(argv[2]);
        if (!input)
        {
            free_envp(envp_copy);
            return (1);
        }
        
        // Execute single command
        tokens = tokenize_input(input);
        if (tokens)
        {
            cmds = parser_tokens(tokens);
            if (cmds)
            {
                cur = cmds;
                fail = 0;
                
                while (cur && !fail)
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
                    
                free_cmds(cmds);
            }
            free_token_list(tokens);
        }
        
        free(input);
        free_envp(envp_copy);
        return (state.last_status);
    }
    
    // Normal interactive mode
    envp_copy = new_envp(envp);
    if (!envp_copy)
        return (1);

    state.last_status = 0;
    
    // Check if stdin is a terminal (interactive mode)
    is_interactive = isatty(STDIN_FILENO);

    while (1)
    {
        fail = 0;
        
        // Only show prompt in interactive mode and get input appropriately
        if (is_interactive)
        {
            input = readline("minishell$ ");
        }
        else
        {
            // For non-interactive mode, read line by line
            char *line = NULL;
            size_t len = 0;
            ssize_t read_len = getline(&line, &len, stdin);
            if (read_len == -1)
            {
                input = NULL; // EOF or error
                if (line)
                    free(line);
            }
            else
            {
                // Remove trailing newline if present
                if (read_len > 0 && line[read_len - 1] == '\n')
                    line[read_len - 1] = '\0';
                input = ft_strdup(line);
                free(line);
            }
        }
            
        if (!input)
            break ; // Ctrl+D or EOF

        if (*input && is_interactive)
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
            free_token_list(tokens);
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
    return (state.last_status);
}