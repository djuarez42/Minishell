/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 16:25:00 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/08/24 13:44:57 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# include <stdbool.h>

typedef struct s_exec_state	t_exec_state;

void						signals_setup_interactive(t_exec_state *state);
void						signals_teardown_interactive(void);

#endif
