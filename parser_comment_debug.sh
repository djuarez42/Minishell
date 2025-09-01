#!/bin/bash

# Temporary file to store changes
TMP_FILE=$(mktemp)

# Process parser.c - Comment out all printf debug statements
sed 's/\(printf.*\[parse_.*\)/\/\/\1/g' /home/ekakhmad/Minishell/src/parser/parser.c > $TMP_FILE
mv $TMP_FILE /home/ekakhmad/Minishell/src/parser/parser.c

# Process parser_print_utils.c - This file is specifically for debug printing, but keep it for later use
# We'll only comment out the main function calls
sed 's/\(printf(".*\)/\/\/\1/g' /home/ekakhmad/Minishell/src/parser/parser_print_utils.c > $TMP_FILE
mv $TMP_FILE /home/ekakhmad/Minishell/src/parser/parser_print_utils.c

# Recompile
cd /home/ekakhmad/Minishell && make
