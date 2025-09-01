#!/bin/bash

# Fix multiple definition of parse_redirections
# Make the version in parser.c static
sed -i 's/t_token\s*\*parse_redirections/static t_token *parse_redirections/' /home/ekakhmad/Minishell/src/parser/parser.c

# Fix multiple definition of print_redirs
# Make the version in parser_utils2.c static
sed -i 's/void\s*print_redirs/static void print_redirs/' /home/ekakhmad/Minishell/src/parser/parser_utils2.c

# Fix undefined references
# Add implementation for create_cmd_node and add_cmd_node to parser_utils.c
