#include "minishell.h"
#include <stdio.h>

int main(int argc, char **argv, char **envp)
{
    t_exec_state state;
    char *input;
    char *expanded;

    (void)argc;
    (void)argv;
    state.last_status = 0;
    input = "$USER";
    expanded = expand_variables(input, envp, &state, QUOTE_NONE);
    printf("Input: %s\n", input);
    printf("Expanded: %s\n", expanded);
    free(expanded);
    return (0);
}
