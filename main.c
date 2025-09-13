/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/13 18:06:08 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "signals.h"
#include <unistd.h> // for isatty()

int main(int argc, char **argv, char **envp)
{
    char        *input;
    t_token     *tokens;
    t_cmd       *cmds = NULL;
    char        **envp_copy;
    t_exec_state state;
    int         fail;
    int         is_interactive;

    envp_copy = new_envp(envp);
    if (!envp_copy)
        return 1;

    state.last_status = 0;
    is_interactive = isatty(STDIN_FILENO);

    // Modo -c
    if (argc >= 3 && ft_strncmp(argv[1], "-c", 3) == 0)
    {
        input = ft_strdup(argv[2]);
        if (!input)
        {
            free_envp(envp_copy);
            return 1;
        }

        tokens = tokenize_input(input);
        if (tokens)
        {
            cmds = parser_tokens(tokens, envp_copy, &state);
            if (cmds)
                executor(cmds, &envp_copy, &state); // <-- sin expand_cmd_inplace
            free_cmds(cmds);
            free_token_list(tokens);
        }
        free(input);
        free_envp(envp_copy);
        return state.last_status;
    }

    // Modo interactivo / batch
    while (1)
    {
        fail = 0;
        if (is_interactive)
            input = readline("minishell$ ");
        else
        {
            char *line = NULL;
            size_t len = 0;
            ssize_t read_len = getline(&line, &len, stdin);
            if (read_len == -1)
            {
                input = NULL;
                if (line) free(line);
            }
            else
            {
                if (read_len > 0 && line[read_len - 1] == '\n')
                    line[read_len - 1] = '\0';
                input = ft_strdup(line);
                free(line);
            }
        }

        if (!input)
            break;

        if (*input && is_interactive)
            add_history(input);

        tokens = tokenize_input(input);
        if (!tokens)
        {
            free(input);
            continue;
        }

        cmds = parser_tokens(tokens, envp_copy, &state);
        if (!cmds)
        {
            free_token_list(tokens);
            free(input);
            continue;
        }

        executor(cmds, &envp_copy, &state); // <-- sin expand_cmd_inplace

        free_token_list(tokens);
        free_cmds(cmds);
        free(input);
    }

    free_envp(envp_copy);
    return state.last_status;
}