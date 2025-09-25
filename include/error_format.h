/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_format.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 17:25:20 by ekakhmad          #+#    #+#             */
/*   Updated: 2025/09/25 17:45:50 by ekakhmad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_FORMAT_H
# define ERROR_FORMAT_H

# include <stdio.h>
# include <string.h>
# include <errno.h>

void	print_error(const char *context, const char *message);
void	print_execve_error(const char *filename);

#endif