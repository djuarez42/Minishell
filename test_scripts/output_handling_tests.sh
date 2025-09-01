#!/usr/bin/env bash
set -euo pipefail
MINISHELL=./minishell

echo "[1] echo hello world"
printf "echo hello world\nexit\n" | $MINISHELL > /tmp/ms_out1.txt 2>/tmp/ms_err1.txt || true
printf "hello world\n" > /tmp/bash_ref1.txt

echo "[2] echo hello | cat"
printf "echo hello | cat\nexit\n" | $MINISHELL > /tmp/ms_out2.txt 2>/tmp/ms_err2.txt || true
printf "hello\n" > /tmp/bash_ref2.txt


echo "[3] redirection echo hello > file ; cat file"
rm -f /tmp/ms_file.txt
printf "echo hello > /tmp/ms_file.txt\ncat /tmp/ms_file.txt\nexit\n" | $MINISHELL > /tmp/ms_out3.txt 2>/tmp/ms_err3.txt || true
printf "hello\n" > /tmp/bash_ref3.txt


echo "[4] pipeline echo hello | grep he"
printf "echo hello | grep he\nexit\n" | $MINISHELL > /tmp/ms_out4.txt 2>/tmp/ms_err4.txt || true
printf "hello\n" > /tmp/bash_ref4.txt


echo "[5] ANSI-C quotes echo $'USER'"
printf "echo $'USER'\nexit\n" | $MINISHELL > /tmp/ms_out5.txt 2>/tmp/ms_err5.txt || true
printf "USER\n" > /tmp/bash_ref5.txt


echo "[6] Locale translation echo $\"USER\""
printf 'echo $"USER"\nexit\n' | $MINISHELL > /tmp/ms_out6.txt 2>/tmp/ms_err6.txt || true
printf "USER\n" > /tmp/bash_ref6.txt


echo "Comparing outputs:"
for i in {1..6}; do
  if ! diff -u "/tmp/ms_out${i}.txt" "/tmp/bash_ref${i}.txt"; then
    echo "Test $i FAILED"
    exit 1
  fi
  echo "Test $i OK"
  if [ -s "/tmp/ms_err${i}.txt" ]; then
    echo "Note: stderr not empty for test $i:" >&2
    cat "/tmp/ms_err${i}.txt" >&2
  fi
done

echo "All output handling tests passed."
