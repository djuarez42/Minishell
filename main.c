/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 20:30:46 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/23 20:54:19 by djuarez          ###   ########.fr       */
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

int main(void)
{
    t_exec_state state = {0};

    // Variables de entorno de prueba
    char *envp[] = {"USER=djuarez", "HOME=/home/djuarez", NULL};

    // --- Prueba de expand_variables ---
    char *inputs[] = {"$USER", "$HOME", "$NONEXISTENT"};
    for (int i = 0; i < 3; i++)
    {
        char *expanded = expand_variables(inputs[i], envp, &state);
        if (!expanded)
        {
            fprintf(stderr, "Error expanding: %s\n", inputs[i]);
            continue;
        }
        printf("Input: %s -> Expanded: %s\n", inputs[i], expanded);
        free(expanded);
    }

    // --- Prueba de expand_redirs ---
    t_redir *r1 = malloc(sizeof(t_redir));
    t_redir *r2 = malloc(sizeof(t_redir));
    if (!r1 || !r2)
        return 1;

    r1->file = strdup("$USER/file.txt");
    r1->next = r2;
    r2->file = strdup("$HOME/file.txt");
    r2->next = NULL;

    if (expand_redirs(r1, envp, &state) == -1)
    {
        fprintf(stderr, "Error expanding redirections\n");
        free_redirs(r1);
        return 1;
    }

    // Mostrar resultados de redirs
    t_redir *cur = r1;
    while (cur)
    {
        printf("Redir file: %s\n", cur->file);
        cur = cur->next;
    }

    // Liberar toda la memoria
    free_redirs(r1);

    return 0;
}
