#!/usr/bin/env bash
set -euo pipefail

# Minishell — Subject Compliance Test Suite (file-driven runner)

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
MINI_SHELL=${MINI_SHELL:-"$ROOT_DIR/minishell"}

# Enhanced binary validation
if [[ ! -f "$MINI_SHELL" ]]; then
  echo "Error: minishell binary not found at $MINI_SHELL" >&2
  echo "Build it first (make)." >&2
  exit 1
fi

if [[ ! -x "$MINI_SHELL" ]]; then
  echo "Error: minishell binary exists but is not executable" >&2
  echo "Try: chmod +x $MINI_SHELL" >&2
  exit 1
fi

# Check file type to detect architecture mismatch
file_info=$(file "$MINI_SHELL" 2>/dev/null || echo "Unknown file type")
if [[ "$file_info" == *"cannot execute binary file"* ]] || [[ "$file_info" == *"Exec format error"* ]]; then
  echo "Error: minishell binary is not executable on this system architecture" >&2
  echo "File info: $file_info" >&2
  echo "You may need to recompile for the current architecture or specify a different binary with:" >&2
  echo "  MINI_SHELL=/path/to/correct/minishell $0" >&2
  exit 1
fi

WORK_BASE=${WORK_BASE:-"$ROOT_DIR/.test_tmp_subject"}
mkdir -p "$WORK_BASE"

# Check if we can actually run the binary before proceeding
if ! "$MINI_SHELL" -c "exit 0" >/dev/null 2>&1; then
  echo "Error: minishell binary cannot be executed properly" >&2
  echo "Try running it manually to check for errors: $MINI_SHELL" >&2
  exit 1
fi

filter_ms_prompt() {
  sed -e 's/^minishell\$ \(.*\)/\1/' -e '/^minishell\$ $/d'
}

# Run same script on bash and minishell using a script file to preserve quoting/heredocs
run_vs_bash() {
  local name="$1"; shift
  local script_content="$1"
  local tdir="$WORK_BASE/$name"
  rm -rf "$tdir" && mkdir -p "$tdir"

  pushd "$tdir" >/dev/null

  # Write script file once
  cat > script.ms <<'MS'
__CONTENT_PLACEHOLDER__
MS
  python3 - <<'PY'
import sys
path = 'script.ms'
content = sys.stdin.read()
with open(path, 'w', encoding='utf-8') as f:
    f.write(content.replace('__CONTENT_PLACEHOLDER__\n', ''))
PY
<<"EOF"
$script_content
EOF

  local b_out b_err b_ec m_out m_err m_ec
  b_out=$(mktemp); b_err=$(mktemp)
  # Run bash with the file content (avoids inline -lc quoting issues)
  bash -lc "$(<"$tdir/script.ms")" >"$b_out" 2>"$b_err" || true
  b_ec=$?

  m_out=$(mktemp); m_err=$(mktemp)
  "$MINI_SHELL" <"$tdir/script.ms" >"$m_out" 2>"$m_err" || true
  m_ec=$?

  local m_out_f m_err_f
  m_out_f=$(mktemp); m_err_f=$(mktemp)
  filter_ms_prompt <"$m_out" >"$m_out_f"
  filter_ms_prompt <"$m_err" >"$m_err_f"

  local ok=0
  if ! diff -u "$b_out" "$m_out_f" >/dev/null; then
    echo "[FAIL] $name: stdout differs"
    echo "--- bash stdout"; cat "$b_out"
    echo "--- minishell stdout"; cat "$m_out_f"
    ok=1
  fi
  if ! diff -u "$b_err" "$m_err_f" >/dev/null; then
    echo "[FAIL] $name: stderr differs"
    echo "--- bash stderr"; cat "$b_err"
    echo "--- minishell stderr"; cat "$m_err_f"
    ok=1
  fi
  if [[ "$b_ec" != "$m_ec" ]]; then
    echo "[FAIL] $name: exit status differs (bash=$b_ec, minishell=$m_ec)"
    ok=1
  fi
  if [[ $ok -eq 0 ]]; then
    echo "[PASS] $name"
  fi

  rm -f "$b_out" "$b_err" "$m_out" "$m_err" "$m_out_f" "$m_err_f"
  popd >/dev/null
  return $ok
}

# Compare minishell stdout to an expected literal (supports \n via %b)
run_expect_stdout() {
  local name="$1"; shift
  local script_content="$1"; shift
  local expected="$1"

  local tdir="$WORK_BASE/$name"
  rm -rf "$tdir" && mkdir -p "$tdir"
  pushd "$tdir" >/dev/null

  printf '%s' "$script_content" > script.ms
  local out_file; out_file=$(mktemp)
  "$MINI_SHELL" < script.ms 2>/dev/null | filter_ms_prompt >"$out_file" || true

  if diff -u <(printf '%b' "$expected") "$out_file" >/dev/null; then
    echo "[PASS] $name"
    popd >/dev/null
    return 0
  else
    echo "[FAIL] $name: stdout mismatch"
    echo "--- expected"; printf '%b' "$expected"
    echo "--- got"; cat "$out_file"
    popd >/dev/null
    return 1
  fi
}

pass=0; fail=0
run_or_count_vs_bash() { if run_vs_bash "$1" "$2"; then pass=$((pass+1)); else fail=$((fail+1)); fi; }
run_or_count_expect()  { if run_expect_stdout "$1" "$2" "$3"; then pass=$((pass+1)); else fail=$((fail+1)); fi; }

# 1) PATH & executables
run_or_count_vs_bash "path_basic" $'ls | head -1\n'
run_or_count_vs_bash "path_unset_then_restore" $'unset PATH\nls || true\nexport PATH=/bin:/usr/bin\nls >/dev/null\n'

# 2) Quotes
run_or_count_vs_bash "single_quotes_literal" $'echo '\''$USER'\''\n'
run_or_count_vs_bash "double_quotes_expand" $'export X=ok\necho "X=$X"\n'

# 3) Pipes
run_or_count_vs_bash "pipes_simple" $'echo hola | grep h\n'
run_or_count_vs_bash "pipes_chain" $'echo hola | cat | grep h | wc -l\n'

# 4) Redirections and heredoc
run_or_count_vs_bash "redir_out_in_append" $'echo a > t1.txt\necho b >> t1.txt\ncat t1.txt\n'
run_or_count_vs_bash "redir_in" $'echo data > in.txt\ncat < in.txt\n'
run_or_count_vs_bash "heredoc_unquoted_expand" $'export V=YES\ncat << EOF\n$V\nEOF\n'
run_or_count_vs_bash "heredoc_quoted_no_expand" $'export V=YES\ncat << '\''EOF'\''\n$V\nEOF\n'

# 5) Env expansion and $?
run_or_count_vs_bash "expand_env_and_status" $'export A=1\necho "$A"\nfalse || true\necho $?\n'

# 6) Builtins
run_or_count_vs_bash "echo_flags" $'echo -n hi; echo X\necho -nnn hi\n'
run_or_count_vs_bash "pwd_and_cd" $'pwd\nmkdir -p d && cd d && pwd\n'
run_or_count_vs_bash "export_unset_env" $'export A=1 B=2\nenv | grep -E '\''^(A|B)='\''\nunset A\nenv | grep -E '\''^(A|B)='\'' | sort\n'
run_or_count_vs_bash "exit_code_42" $'exit 42\n'

# 7) Subject-specific differences
run_or_count_expect "semicolon_not_separator" $'echo foo; echo bar\n' $'foo; echo bar\n'
run_or_count_expect "backslash_literal"        $'echo a\\ b\n'      $'a\\ b\n'

echo
echo "Subject suite: PASS=$pass FAIL=$fail"
exit $(( fail > 0 ))

# -----------------------------------------------------------------------------
# Appendix: All tested cases and their command snippets (manual copy/paste)
#
# path_basic
#   ls | head -1
#
# path_unset_then_restore
#   unset PATH
#   ls || true
#   export PATH=/bin:/usr/bin
#   ls >/dev/null
#
# single_quotes_literal
#   echo '$USER'
#
# double_quotes_expand
#   export X=ok
#   echo "X=$X"
#
# pipes_simple
#   echo hola | grep h
#
# pipes_chain
#   echo hola | cat | grep h | wc -l
#
# redir_out_in_append
#   echo a > t1.txt
#   echo b >> t1.txt
#   cat t1.txt
#
# redir_in
#   echo data > in.txt
#   cat < in.txt
#
# heredoc_unquoted_expand
#   export V=YES
#   cat << EOF
#   $V
#   EOF
#
# heredoc_quoted_no_expand
#   export V=YES
#   cat << 'EOF'
#   $V
#   EOF
#
# expand_env_and_status
#   export A=1
#   echo "$A"
#   false || true
#   echo $?
#
# echo_flags
#   echo -n hi; echo X
#   echo -nnn hi
#
# pwd_and_cd
#   pwd
#   mkdir -p d && cd d && pwd
#
# export_unset_env
#   export A=1 B=2
#   env | grep -E '^(A|B)='
#   unset A
#   env | grep -E '^(A|B)=' | sort
#
# exit_code_42
#   exit 42
#
# semicolon_not_separator
#   echo foo; echo bar
#   # expected literal: foo; echo bar\n
# backslash_literal
#   echo a\ b
#   # expected literal: a\ b\n

