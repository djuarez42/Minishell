#!/usr/bin/env bash
set -euo pipefail
# Contributor B Tests - Lexer/parser comment handling, PWD/OLDPWD updates, and error formatting
# Focus: env_print (comments), cd_and_pwd (PWD/OLDPWD updates), missing_cmd (error format)

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
MINI_SHELL=${MINI_SHELL:-"$ROOT_DIR/minishell"}

if [[ ! -x "$MINI_SHELL" ]]; then
  echo "Error: minishell binary not found or not executable at $MINI_SHELL" >&2
  echo "Build it first (make)." >&2
  exit 1
fi

# Work area per test
WORK_BASE=${WORK_BASE:-"$ROOT_DIR/.test_tmp"}
mkdir -p "$WORK_BASE"

# Filter minishell prompt from output if readline prints it when not attached to a TTY
filter_ms_prompt() {
  # Remove standalone prompt lines and leading prompt tokens
  sed -e 's/^minishell\$ \(.*\)/\1/' -e '/^minishell\$ $/d'
}

# Run a small script in a temp directory with both bash and minishell
# Args: test_name, script (string)
run_case() {
  local name="$1"; shift
  local script="$1"
  local tdir="$WORK_BASE/$name"
  rm -rf "$tdir" && mkdir -p "$tdir"

  # Prepare a clean environment for both runs (inherit host env but isolate PWD)
  # Bash run
  pushd "$tdir" >/dev/null
  local b_out b_err b_ec m_out m_err m_ec

  # Bash: run in a login-like shell with -lc to process multiple lines consistently
  b_out=$(mktemp)
  b_err=$(mktemp)
  bash -lc "$script" >"$b_out" 2>"$b_err" || true
  b_ec=$?

  # Minishell: feed the same script via heredoc
  m_out=$(mktemp)
  m_err=$(mktemp)
  # shellcheck disable=SC2002
  cat >"$tdir/script.ms" <<'MS'
__SCRIPT_CONTENT_PLACEHOLDER__
MS
  # Replace placeholder with the actual script safely
  # Use printf %s to avoid interpretation
  python3 - "$tdir/script.ms" "$script" <<'PY'
import sys
path, content = sys.argv[1], sys.argv[2]
with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
PY
  "$MINI_SHELL" <"$tdir/script.ms" >"$m_out" 2>"$m_err" || true
  m_ec=$?
  # Filter potential prompts
  local m_out_f m_err_f
  m_out_f=$(mktemp)
  m_err_f=$(mktemp)
  filter_ms_prompt <"$m_out" >"$m_out_f"
  filter_ms_prompt <"$m_err" >"$m_err_f"

  # Compare
  local ok=1
  if ! diff -u "$b_out" "$m_out_f" >/dev/null; then
    echo "[FAIL] $name: stdout differs"
    echo "--- bash stdout"; cat "$b_out"
    echo "--- minishell stdout"; cat "$m_out_f"
    ok=0
  fi
  if ! diff -u "$b_err" "$m_err_f" >/dev/null; then
    echo "[FAIL] $name: stderr differs"
    echo "--- bash stderr"; cat "$b_err"
    echo "--- minishell stderr"; cat "$m_err_f"
    ok=0
  fi
  if [[ "$b_ec" != "$m_ec" ]]; then
    echo "[FAIL] $name: exit status differs (bash=$b_ec, minishell=$m_ec)"
    ok=0
  fi
  if [[ $ok -eq 1 ]]; then
    echo "[PASS] $name"
  fi

  # Cleanup temps
  rm -f "$b_out" "$b_err" "$m_out" "$m_err" "$m_out_f" "$m_err_f"
  popd >/dev/null
  return $ok
}

# Helper to build a here-document for the case
ms() { cat; }

# CONTRIBUTOR B TESTS
echo "=== Contributor B Tests - Lexer/parser, PWD/OLDPWD updates, error formatting ==="

pass=0; fail=0
run_or_count() {
  local name="$1"; shift
  local script="$1"
  if run_case "$name" "$script"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
  fi
}

# 1) Comment handling - env_print should handle # comments properly
run_or_count "env_print" "$(ms <<'SH'
# Normalize by grepping a known var we set
export FOO=bar
env | grep '^FOO='
SH
)"

# 2) PWD/OLDPWD updates - cd should update PWD/OLDPWD environment variables (mandatory only)
run_or_count "cd_and_pwd" "$(ms <<'SH'
pwd
mkdir -p sub
cd sub
pwd
SH
)"

# 3) Missing command error formatting - should match bash format
run_or_count "missing_cmd" "$(ms <<'SH'
no_such_cmd arg1
SH
)"

# 4) echo -n command parsing (moved from A)
run_or_count "echo_n" "$(ms <<'SH'
echo -n "no-newline"
printf "<END>\n"
SH
)"

# 5) cd with missing operand (moved from A)
run_or_count "cd_missing" "$(ms <<'SH'
cd
SH
)"

# Summary
echo
echo "=== Contributor B Summary ==="
echo "PASS=$pass FAIL=$fail"
if [[ $fail -gt 0 ]]; then
  echo "Issues to fix:"
  echo "1. Comment handling: # should be treated as comment start (not executed as command)"
  echo "2. PWD/OLDPWD updates: cd should update both PWD and OLDPWD environment variables"
  echo "3. Missing command error: format should match 'bash: line 1: <cmd>: command not found'"
  echo "4. echo -n command parsing: multiple commands should execute in sequence"
  echo "5. cd missing operand: suppress custom error message"
  exit 1
fi
echo "All Contributor B tests passed!"
exit 0
