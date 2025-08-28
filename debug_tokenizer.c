#include "include/minishell.h"

int main()
{
    char *input = "echo hello > /tmp/test";
    t_token *tokens = tokenize_input(input);
    
    printf("=== TOKENIZATION DEBUG ===\n");
    t_token *cur = tokens;
    while (cur)
    {
        printf("Token: '%s' | Type: %s\n", 
               cur->value ? cur->value : "NULL", 
               token_type_str(cur->type));
        cur = cur->next;
    }
    
    free_token_list(tokens);
    return 0;
}
