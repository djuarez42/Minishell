#!/usr/bin/env bash
set -euo pipefail
# Contributor A Tests - Builtins correctness and environment behavior
# Focus: echo_n, export_unset, export_name_only, unset_invalid, cd_missing

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

# CONTRIBUTOR A TESTS
echo "=== Contributor A Tests - Builtins correctness and environment behavior ==="

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

# 1) export behavior alignment
run_or_count "export_unset" "$(ms <<'SH'
export A=1 B=two
env | grep -E '^(A|B)='
unset A
env | grep -E '^(A|B)=' | sort
SH
)"

# 2) export name only (should not produce output unless -p is used)
run_or_count "export_name_only" "$(ms <<'SH'
unset ZED
export ZED
env | grep '^ZED='
SH
)"

# 3) unset validation policy (should not emit error for invalid identifiers in this context)
run_or_count "unset_invalid" "$(ms <<'SH'
unset 1INVALID
SH
)"

# Summary
echo
echo "=== Contributor A Summary ==="
echo "PASS=$pass FAIL=$fail"
if [[ $fail -gt 0 ]]; then
  echo "Issues to fix:"
  echo "1. export behavior: Variable ordering and export NAME without = should not produce output"
  echo "2. unset validation: suppress error messages for invalid identifiers"
  exit 1
fi
echo "All Contributor A tests passed!"
exit 0
