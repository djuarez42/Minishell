# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ekakhmad <ekakhmad@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/28 16:09:51 by djuarez           #+#    #+#              #c
#    Updated: 2025/08/25 20:20:28 by ekakhmad         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = minishell

CC = cc 
CFLAGS = -Wall -Wextra -Werror -g #-fsanitize=address -g

INCLUDES = -Iinclude -Ilibft

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Detect OS to link readline correctly
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
		src/lexer/lexer_utils.c \
		src/lexer/tokenizer_utils.c \
		src/lexer/token_lexer_utils.c \
		src/lexer/tokenizer_utils_2.c \
		src/lexer/lexer_utils_3.c \
		src/lexer/lexer_utils_4.c \
		src/parser/parser.c \
		src/parser/parser_utils.c \
		src/parser/parser_utils2.c \
		src/parser/parser_print_utils.c \
		src/executor/executor.c \
		src/executor/executor_utils.c \
		src/executor/env_utils.c \
		src/executor/redir_utils.c \
		src/executor/redir_utils2.c \
		src/executor/expand_variables_utils.c \
		src/executor/expand_variables_utils2.c \
		src/executor/expand_variables.c \
		src/executor/expand_mixed_quotes.c \
		src/executor/preprocess_dollar_quotes.c \
		src/builtins/builtins.c \
		src/builtins/builtin_echo_pwd_env.c \
		src/builtins/builtin_cd.c \
		src/builtins/builtin_export.c \
		src/builtins/builtin_unset_exit.c \
		src/signals/signals.c 
		

OBJ = $(SRC:.c=.o)

%.o: %.c Makefile 
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)
	
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) -o $(NAME) $(LIBS) $(INCLUDES)

clean:
	rm -f $(OBJ)
	$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	$(MAKE) fclean -C $(LIBFT_DIR)
	
re: fclean all

# Run builtin tests
.PHONY: test-builtin
test-builtin: $(NAME)
	@bash ./tests/builtins_tests.sh

# Run contributor-specific tests
.PHONY: test-a test-b
test-a: $(NAME)
	@bash ./tests/contributor_a_tests.sh

test-b: $(NAME)
	@bash ./tests/contributor_b_tests.sh

# Smoke tests: mandatory-only quick suite
.PHONY: test-smoke
test-smoke: $(NAME)
	@bash ./tests/smoke_tests.sh

.PHONY: all clean fclean re

.PHONY: test-subject
test-subject: $(NAME)
	@bash ./tests/subject_full_tests.sh
