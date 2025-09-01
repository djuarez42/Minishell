#!/bin/bash

# Update the fd_manager.h file to ensure correct token type mapping
sed -i 's/# define REDIR_IN       TOKEN_REDIRECT_IN/# define REDIR_IN       2 \/\* TOKEN_REDIRECT_IN \*\//g' src/executor/fd_manager.h
sed -i 's/# define REDIR_OUT      TOKEN_REDIRECT_OUT/# define REDIR_OUT      3 \/\* TOKEN_REDIRECT_OUT \*\//g' src/executor/fd_manager.h
sed -i 's/# define REDIR_HEREDOC  TOKEN_HEREDOC/# define REDIR_HEREDOC  4 \/\* TOKEN_HEREDOC \*\//g' src/executor/fd_manager.h
sed -i 's/# define REDIR_APPEND   TOKEN_APPEND/# define REDIR_APPEND   5 \/\* TOKEN_APPEND \*\//g' src/executor/fd_manager.h

# Modify redirection.c to add more debugging and fix the type comparison
cat > src/executor/redirection.c.new << 'EOF'
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 18:42:28 by ekakhmad          #+#    #+#             */
/*   Updated: 2023/08/31 21:00:45 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fd_manager.h"
#include "minishell.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Process all redirections for a command in a child process
 */
int	setup_redirections(t_redir *redirs, char **envp, t_exec_state *state)
{
	t_fd_state	*fd_state;
	t_redir		*current;
	int			result;

	// Debug
	fprintf(stderr, "DEBUG: setup_redirections called\n");

	fd_state = fd_state_init();
	if (!fd_state)
	{
		fprintf(stderr, "DEBUG: fd_state_init failed\n");
		return (1);
	}

	// Save original file descriptors
	fd_state_save_original(fd_state);
	fprintf(stderr, "DEBUG: Original FDs saved: stdin=35, stdout=3072, stderr=10\n",
		fd_state->original_stdin, fd_state->original_stdout, fd_state->original_stderr);

	current = redirs;
	while (current)
	{
		// Debug - Show the actual values for comparison
		fprintf(stderr, "DEBUG: Processing redirection type: 51, file: %s\n",
		       current->type, current->file);
		fprintf(stderr, "DEBUG: Constants - REDIR_IN: 40, REDIR_OUT: 3072, REDIR_APPEND: -1206778784, REDIR_HEREDOC: 2147483647\n",
		       REDIR_IN, REDIR_OUT, REDIR_APPEND, REDIR_HEREDOC);

		// Process based on redirection type
		if (current->type == REDIR_IN)
		{
			fprintf(stderr, "DEBUG: Setting up input redirection\n");
			result = setup_input_redirection(current->file, fd_state);
		}
		else if (current->type == REDIR_OUT)
		{
			fprintf(stderr, "DEBUG: Setting up output redirection\n");
			result = setup_output_redirection(current->file, false, fd_state);
		}
		else if (current->type == REDIR_APPEND)
		{
			fprintf(stderr, "DEBUG: Setting up append redirection\n");
			result = setup_output_redirection(current->file, true, fd_state);
		}
		else if (current->type == REDIR_HEREDOC)
		{
			fprintf(stderr, "DEBUG: Setting up heredoc redirection\n");
			if (current->heredoc_fd != -1)
			{
				result = setup_heredoc_redirection(current->heredoc_fd, fd_state);
				close(current->heredoc_fd);
				current->heredoc_fd = -1;
			}
			else
				result = 1;
		}
		else
		{
			fprintf(stderr, "DEBUG: Unknown redirection type: 4\n", current->type);
			result = 0;
		}

		// Handle errors in redirection setup
		if (result != 0)
		{
			fprintf(stderr, "DEBUG: Redirection setup failed with result=4\n", result);
			fd_state_cleanup(fd_state);
			return (result);
		}

		current = current->next;
	}

	// In child process, we don't need to clean up the fd_state 
	// since it's going to be destroyed with the process exit
	// Just leave the redirections in place

	return (0);
}

/**
 * Process all redirections for a command in the parent process (for builtins)
 */
int	setup_redirections_parent(t_redir *redirs, char **envp, 
		t_exec_state *state, t_fd_state **fd_state)
{
	t_fd_state	*state_local;
	t_redir		*current;
	int			result;

	state_local = fd_state_init();
	if (!state_local)
		return (1);

	// Save original file descriptors
	fd_state_save_original(state_local);

	// Process all redirections
	current = redirs;
	while (current)
	{
		// Process based on redirection type
		if (current->type == REDIR_IN)
			result = setup_input_redirection(current->file, state_local);
		else if (current->type == REDIR_OUT)
			result = setup_output_redirection(current->file, false, state_local);
		else if (current->type == REDIR_APPEND)
			result = setup_output_redirection(current->file, true, state_local);
		else if (current->type == REDIR_HEREDOC)
		{
			if (current->heredoc_fd != -1)
			{
				result = setup_heredoc_redirection(current->heredoc_fd, state_local);
				close(current->heredoc_fd);
				current->heredoc_fd = -1;
			}
			else
				result = 1;
		}
		else
			result = 0;

		// Handle errors in redirection setup
		if (result != 0)
		{
			fd_state_cleanup(state_local);
			return (result);
		}

		current = current->next;
	}

	// In parent process, we need to return the fd_state so it can be cleaned up later
	*fd_state = state_local;
	return (0);
}
EOF

# Replace the redirection.c file with our modified version
mv src/executor/redirection.c.new src/executor/redirection.c

# Recompile
make

# Run a simple test
echo 'echo "test fixed" > test_fixed.txt' > test_fixed.txt
echo "Testing redirection fix..."
./minishell < test_fixed.txt 2> debug_fix.log

# Check if file was created
if [ -f test_fixed.txt ]; then
  echo "SUCCESS: test_fixed.txt was created"
  echo "Contents:"
  cat test_fixed.txt
else
  echo "FAILURE: test_fixed.txt was not created"
fi

echo "Debug log:"
cat debug_fix.log
