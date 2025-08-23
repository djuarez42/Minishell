/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/07 17:00:07 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/23 22:44:22 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	add_cmd_node(t_cmd **head, t_cmd **last, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*last)->next = new_cmd;
	*last = new_cmd;
}

t_cmd	*create_cmd_node(t_token **cur)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->argv_quote = NULL;
	cmd->redirs = NULL;
	cmd->pipe = 0;
	cmd->next = NULL;
	*cur = parse_cmd_block(*cur, cmd);
	if (!*cur)
	/* 	return (free(cmd), NULL); */
        free_cmds(cmd);
        return (NULL);
	if ((*cur)->type == TOKEN_PIPE)
	{
		cmd->pipe = 1;
		*cur = (*cur)->next;
	}
	return (cmd);
}

t_cmd	*parser_tokens(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*last;
	t_token	*cur;
	t_cmd	*new_cmd;

	head = NULL;
	last = NULL;
	cur = tokens;
	while (cur && cur->type != TOKEN_EOF)
	{
		new_cmd = create_cmd_node(&cur);
		if (!new_cmd)
			 return (free_cmds(head), NULL);
		add_cmd_node(&head, &last, new_cmd);
	}
	return (head);
}
/* You've found the exact location of the leak. The problem is in your `create_cmd_node` function.

### The Problem:

In `create_cmd_node`, you have this error handling:
```c


// ...existing code...
	*cur = parse_cmd_block(*cur, cmd);
	if (!*cur)
		return (free(cmd), NULL); // <-- THIS IS THE LEAK
// ...existing code...
```

When `parse_cmd_block` fails (e.g., on a syntax error like `echo >`), it returns `NULL`. However, before it failed, it had already called `parse_arguments` which successfully allocated memory for `cmd->argv` and filled it with strings.

Your code only calls `free(cmd)`, which frees the `t_cmd` struct itself, but it **leaks**:
*   The `cmd->argv` array.
*   All the strings inside `cmd->argv`.
*   The `cmd->argv_quote` array.
*   Any redirections that might have been partially parsed.

### The Fix:

You need to call your full command cleanup function instead of just `free(cmd)`. Since `free_cmds` frees a whole list, let's adapt it to free the single, partially-built `cmd` node.

```c


// ...existing code...
t_cmd	*create_cmd_node(t_token **cur)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
// ...existing code...
	cmd->pipe = 0;
	cmd->next = NULL;
	*cur = parse_cmd_block(*cur, cmd);
	if (!*cur)
	{
		// Use the full cleanup logic, not just free(cmd)
		free_cmds(cmd);
		return (NULL);
	}
	if ((*cur)->type == TOKEN_PIPE)
// ...existing code...
```

By replacing `(free(cmd), NULL)` with a call to `free_cmds(cmd)`, you ensure that all the memory allocated for the failed command node (including its `argv` and `redirs`) is properly cleaned up. This will resolve the remaining 1,200-byte leak. */
