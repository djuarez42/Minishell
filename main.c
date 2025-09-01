/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/09/01 11:01:47 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include "signals.h"
#include <unistd.h> // for isatty()
#include <stdio.h>  // for getline

int process_command(char *input, char ***envp_copy, t_exec_state *state)
{
    t_token *tokens;
    t_cmd *cmds;
    t_cmd *cur;
    int fail = 0;
    
    if (!input || !*input)
        return 0;
        
    // 1. Lexer: tokens limpios listos para parser
    tokens = tokenize_input(input);
    if (!tokens)
        return 0;

    // 2. Parser: construye lista de comandos
    cmds = parser_tokens(tokens);
    if (!cmds)
    {
        free_token_list(tokens);
        return 0;
    }

    // 3. Expansión de variables / sustituciones
    cur = cmds;
    while (cur)
    {
        if (expand_cmd_inplace(cur, *envp_copy, state) == -1)
        {
            fail = 1;
            break;
        }
        cur = cur->next;
    }

    // 4. Ejecutar
    if (!fail)
        executor(cmds, envp_copy, state);

    // 5. Liberar memoria
    free_token_list(tokens);
    free_cmds(cmds);
    
    return (!fail);
}

int main(int argc, char **argv, char **envp)
{
    char **envp_copy;
    t_exec_state state;
    int is_interactive;

    // Check for -c option (execute command and exit)
    if (argc >= 3 && ft_strncmp(argv[1], "-c", 3) == 0)
    {
        // Running in non-interactive mode with -c option
        envp_copy = new_envp(envp);
        if (!envp_copy)
            return (1);
            
        state.last_status = 0;
        
        // Process the command
        char *input = ft_strdup(argv[2]);
        if (!input)
        {
            free_envp(envp_copy);
            return (1);
        }
        
        process_command(input, &envp_copy, &state);
        
        free(input);
        free_envp(envp_copy);
        return (state.last_status);
    }
    
    // Normal mode (interactive or piped)
    envp_copy = new_envp(envp);
    if (!envp_copy)
        return (1);

    state.last_status = 0;
    
    // Check if stdin is a terminal (interactive mode)
    is_interactive = isatty(STDIN_FILENO);

    // Buffer for reading lines from stdin
    char *line = NULL;
    size_t line_cap = 0;
    ssize_t line_len;
    
    while (1)
    {
        // Only show prompt in interactive mode
        if (is_interactive)
        {
            char *input = readline("minishell$ ");
            if (!input)
                break; // Ctrl+D or EOF
                
            if (*input)
                add_history(input);
                
            // Process the command
            process_command(input, &envp_copy, &state);
            free(input);
        }
        else
        {
            // Non-interactive mode: read lines from stdin
            line_len = getline(&line, &line_cap, stdin);
            if (line_len <= 0)
                break; // EOF or error
                
            // Remove trailing newline if present
            if (line_len > 0 && line[line_len - 1] == '\n')
                line[line_len - 1] = '\0';
                
            // Process the command
            process_command(line, &envp_copy, &state);
        }
    }
    
    // Cleanup
    if (line)
        free(line);
    free_envp(envp_copy);
    return (state.last_status);
}