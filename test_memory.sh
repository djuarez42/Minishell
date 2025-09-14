#!/bin/bash
# test_memory.sh - versión segura para heredocs

MINISHELL=./minishell

echo "=== TEST: comandos simples ==="
printf "echo hola\npwd\nexit\n" | $MINISHELL

echo "=== TEST: variables de entorno ==="
printf "export TEST=123\necho \$TEST\nunset TEST\nexit\n" | $MINISHELL

echo "=== TEST: pipes simples ==="
printf "echo hola | cat\nls | wc -l\nexit\n" | $MINISHELL

echo "=== TEST: redirecciones básicas ==="
printf "echo hola > file.txt\ncat < file.txt\nrm file.txt\nexit\n" | $MINISHELL

echo "=== TEST: heredoc simple ==="
printf "cat <<EOF\nLinea1\nLinea2\nEOF\nexit\n" | $MINISHELL

echo "=== TEST: built-ins extremos ==="
printf "export A=1\nexport B=2\nenv\nunset A\nunset B\nexit\n" | $MINISHELL

echo "=== TEST: entrada vacía y espacios ==="
printf "   \n    ls   \nexit\n" | $MINISHELL

echo "=== FIN DE PRUEBAS ==="
