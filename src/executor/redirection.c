/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 18:42:28 by ekakhmad          #+#    #+#             */
/*   Updated: 2023/08/31 19:30:45 by ekakhmad         ###   ########.fr       */
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
    
    (void)envp;  /* Unused parameter */
    (void)state; /* Unused parameter */

	/* Debug */
	fprintf(stderr, "DEBUG: setup_redirections called\n");
	
	if (redirs == NULL) {
		fprintf(stderr, "DEBUG: No redirections to process\n");
		return 0;
	}

	fd_state = fd_state_init();
	if (!fd_state)
	{
		fprintf(stderr, "DEBUG: fd_state_init failed\n");
		return (1);
	}

	/* Save original file descriptors */
	fd_state_save_original(fd_state);
	fprintf(stderr, "DEBUG: Original FDs saved: stdin=%d, stdout=%d, stderr=%d\n", 
		fd_state->original_stdin, fd_state->original_stdout, fd_state->original_stderr);

	current = redirs;
	while (current)
	{
		/* Debug */
		fprintf(stderr, "DEBUG: Processing redirection type: %d, file: %s\n", 
		       current->type, current->file ? current->file : "NULL");
		
		/* Process based on redirection type */
		if (current->type == REDIR_IN)
		{
			fprintf(stderr, "DEBUG: Setting up input redirection (REDIR_IN=%d)\n", REDIR_IN);
			result = setup_input_redirection(current->file, fd_state);
		}
		else if (current->type == REDIR_OUT)
		{
			fprintf(stderr, "DEBUG: Setting up output redirection (REDIR_OUT=%d)\n", REDIR_OUT);
			result = setup_output_redirection(current->file, false, fd_state);
		}
		else if (current->type == REDIR_APPEND)
		{
			fprintf(stderr, "DEBUG: Setting up append redirection (REDIR_APPEND=%d)\n", REDIR_APPEND);
			result = setup_output_redirection(current->file, true, fd_state);
		}
		else if (current->type == REDIR_HEREDOC)
		{
			fprintf(stderr, "DEBUG: Setting up heredoc redirection (REDIR_HEREDOC=%d)\n", REDIR_HEREDOC);
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
			fprintf(stderr, "DEBUG: Unknown redirection type: %d\n", current->type);
			result = 0;
		}

		/* Handle errors in redirection setup */
		if (result != 0)
		{
			fd_state_cleanup(fd_state);
			return (result);
		}

		current = current->next;
	}

	/* In child process, we don't need to clean up the fd_state 
	   since it's going to be destroyed with the process exit
	   Just leave the redirections in place */

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
    
    (void)envp;  /* Unused parameter */
    (void)state; /* Unused parameter */

	/* Debug */
	fprintf(stderr, "DEBUG: setup_redirections_parent called\n");
	
	if (redirs == NULL) {
		fprintf(stderr, "DEBUG: No redirections to process (parent)\n");
		return 0;
	}

	state_local = fd_state_init();
	if (!state_local)
		return (1);

	/* Save original file descriptors */
	fd_state_save_original(state_local);

	/* Process all redirections */
	current = redirs;
	while (current)
	{
		/* Debug */
		fprintf(stderr, "DEBUG: Processing redirection type (parent): %d, file: %s\n", 
		       current->type, current->file ? current->file : "NULL");
		
		/* Process based on redirection type */
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

		/* Handle errors in redirection setup */
		if (result != 0)
		{
			fd_state_cleanup(state_local);
			return (result);
		}

		current = current->next;
	}

	/* In parent process, we need to return the fd_state so it can be cleaned up later */
	*fd_state = state_local;
	return (0);
}
