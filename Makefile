# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ekakhmad <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/28 16:09:51 by djuarez           #+#    #+#              #
#    Updated: 2025/09/27 21:31:40 by ekakhmad         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = minishell

CC = cc 
CFLAGS = -Wall -Wextra -Werror -g #-fsanitize=address
#LDFLAGS = -fsanitize=address


INCLUDES = -Iinclude -Ilibft

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	# macOS (Homebrew usually provides readline in default search paths)
	READLINE_LIBS = -lreadline
	READLINE_INC  =
else
	# Linux/Ubuntu: readline commonly requires ncurses (or tinfo)
	READLINE_LIBS = -lreadline -lncurses -ltinfo
	READLINE_INC  =
endif

LIBS = $(READLINE_LIBS)
INCLUDES += $(READLINE_INC)

SRC = main.c \
		src/lexer/tokenizer.c \
		src/lexer/lexer_utils1.c \
		src/lexer/lexer_utils2.c \
		src/lexer/lexer_utils3.c \
		src/lexer/lexer_utils4.c \
		src/lexer/lexer_utils5.c \
		src/lexer/lexer_utils6.c \
		src/lexer/lexer_utils7.c \
		src/lexer/lexer_utils8.c \
		src/parser/parser.c \
		src/parser/parser_utils.c \
		src/parser/parser_utils1.c \
		src/parser/parser_utils2.c \
		src/parser/parser_utils3.c \
		src/parser/parser_utils4.c \
		src/parser/parser_utils5.c \
		src/parser/parser_utils6.c \
		src/parser/parser_utils7.c \
		src/parser/parser_utils8.c \
		src/parser/parser_utils9.c \
		src/parser/parser_utils10.c \
		src/parser/parser_utils11.c \
		src/parser/parser_utils12.c \
		src/parser/parser_utils13.c \
		src/executor/executor.c \
		src/executor/executor_utils.c \
		src/parser/parser_helpers.c \
		src/executor/executor_utils1.c \
		src/executor/executor_utils2.c \
		src/executor/executor_utils3.c \
		src/executor/executor_utils4.c \
		src/executor/executor_utils5.c \
		src/executor/executor_utils6.c \
		src/executor/executor_utils7.c \
		src/executor/executor_utils8.c \
		src/executor/executor_utils9.c \
		src/executor/executor_utils10.c \
		src/executor/executor_utils11.c \
		src/executor/executor_utils12.c \
		src/executor/executor_utils13.c \
		src/executor/executor_utils14.c \
		src/executor/executor_helpers.c \
		src/executor/executor_pipe_helpers.c \
		src/executor/executor_redir_helpers.c \
		src/executor/executor_heredoc_helpers.c \
		src/executor/executor_utils5_helpers.c \
		src/executor/executor_utils6_helpers.c \
		src/executor/executor_itoa_helpers.c \
		src/executor/executor_utils15.c \
		src/executor/executor_utils16.c \
		src/executor/executor_utils17.c \
		src/executor/executor_utils18.c \
		src/executor/executor_utils19.c \
		src/builtins/builtins.c \
		src/builtins/builtin_echo_pwd_env.c \
		src/builtins/builtin_cd.c \
	src/builtins/builtin_export.c \
		src/builtins/export_print_helpers.c \
		src/builtins/export_utils_print_helpers.c \
		src/builtins/export_utils_names.c \
		src/builtins/export_utils_print.c \
		src/builtins/export_utils_print2.c \
		src/builtins/export_utils1.c \
		src/builtins/export_utils2.c \
		src/builtins/export_utils3.c \
		src/builtins/export_utils4.c \
		src/builtins/export_utils5.c \
		src/builtins/builtin_unset_exit.c \
		src/builtins/unset_exit_helpers.c \
	src/error_format.c \

OBJ = $(SRC:.c=.o)

%.o: %.c Makefile 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
	
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) -o $(NAME) $(LIBS) $(INCLUDES) $(LDFLAGS)

clean:
	rm -f $(OBJ)
	$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	$(MAKE) fclean -C $(LIBFT_DIR)
	
re: fclean all

.PHONY: all clean fclean re
