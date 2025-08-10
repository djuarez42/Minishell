/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djuarez <djuarez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 18:27:06 by djuarez           #+#    #+#             */
/*   Updated: 2025/08/06 20:03:12 by djuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

//Standar libreries for minishell project
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <stdbool.h>

//Librerias para el proyecto minishell
# include <readline/readline.h>
# include <readline/history.h>

// Otros headers del proyecto (por ahora, solo el lexer)
# include "lexer.h"
# include "parser.h"
# include "executor.h"
# include "libft.h"

//Constantes globales si las necesitas
# define PROMPT "minishell$ "

//Funciton prototypes

#endif