#!/bin/bash

# Compila tu minishell con símbolos de depuración
make re CFLAGS="-Wall -Wextra -Werror -g3"

echo "===== PRUEBA FOCALIZADA: EXPANSIÓN DE VARIABLES Y REDIRECCIONES ====="

# Archivo temporal con comandos que suelen generar la fuga
TMPFILE=$(mktemp)

cat > "$TMPFILE" <<'EOF'
# Variables simples
echo $USER
echo $HOME

# Redirección con comillas (fuente de los 14 bytes perdidos)
echo "Comillas" > /tmp/testfile
cat /tmp/testfile
rm /tmp/testfile

# Concatenación de variables y texto
echo "Path: $PATH end"

# Variable inexistente
echo $NOEXISTE

exit
EOF

# Ejecuta minishell bajo valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
         --log-file=valgrind_expand_focus.log \
         ./minishell < "$TMPFILE"

# Mostrar resumen de valgrind
echo "===== SALIDA DE VALGRIND ====="
grep -A10 "LEAK" valgrind_expand_focus.log || echo "No hay fugas detectadas :)"

# Limpiar archivo temporal
rm "$TMPFILE"

