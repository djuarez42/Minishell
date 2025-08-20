#!/bin/bash

# === TEST ECHO ===
echo "=== TEST echo ==="
echo hola
echo -n "sin salto"
echo ""

# === TEST PWD ===
echo "=== TEST pwd ==="
pwd

# === TEST CD ===
echo "=== TEST cd ==="
pwd
cd /
pwd
cd -
pwd

# === TEST EXPORT y ENV ===
echo "=== TEST export y env ==="
export TESTVAR=42
echo $TESTVAR
env | grep TESTVAR

# === TEST UNSET ===
echo "=== TEST unset ==="
unset TESTVAR
echo $TESTVAR

# === TEST BUILTINS COMBINADOS ===
echo "=== TEST echo + pipes ==="
echo hola mundo | tr a-z A-Z

# === TEST REDIRECCIONES ===
echo "=== TEST > redirection ==="
echo "contenido" > test_output.txt
cat test_output.txt

echo "=== TEST >> append ==="
echo "más contenido" >> test_output.txt
cat test_output.txt

echo "=== TEST < input redirection ==="
cat < test_output.txt

# === TEST EXIT ===
echo "=== TEST exit ==="
exit

