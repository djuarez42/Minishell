/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 16:25:10 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/20 16:25:10 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdlib.h>
#include "minishell.h"
#include "signals.h"

static struct termios	g_termios_orig;
static bool			g_termios_saved = false;
static t_exec_state	*g_state_ptr = NULL;

static void	set_echoctl(bool enable)
{
    struct termios t;

    if (!isatty(STDIN_FILENO))
        return ;
    if (tcgetattr(STDIN_FILENO, &t) == -1)
        return ;
    if (!enable)
        t.c_lflag &= ~ECHOCTL;
    else
        t.c_lflag |= ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

static void	sigint_handler(int signo)
{
    (void)signo;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    if (g_state_ptr)
        g_state_ptr->last_status = 130;
}

void	signals_setup_interactive(t_exec_state *state)
{
    struct sigaction sa;

    g_state_ptr = state;
    if (isatty(STDIN_FILENO) && !g_termios_saved)
    {
        if (tcgetattr(STDIN_FILENO, &g_termios_orig) == 0)
            g_termios_saved = true;
    }
    rl_catch_signals = 0;
    set_echoctl(false);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    signal(SIGQUIT, SIG_IGN);
}

void	signals_teardown_interactive(void)
{
    if (g_termios_saved)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_termios_orig);
        g_termios_saved = false;
    }
}


