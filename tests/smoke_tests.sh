#!/usr/bin/env bash
set -euo pipefail

# Smoke tests: fast mandatory-only checks that avoid bonus operators like ';' and '&&'

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
MINI_SHELL=${MINI_SHELL:-"$ROOT_DIR/minishell"}

if [[ ! -x "$MINI_SHELL" ]]; then
  echo "Error: minishell binary not found or not executable at $MINI_SHELL" >&2
  echo "Build it first (make)." >&2
  exit 1
fi

WORK_BASE=${WORK_BASE:-"$ROOT_DIR/.test_tmp_smoke"}
mkdir -p "$WORK_BASE"

filter_ms_prompt() {
  sed -e 's/^minishell\$ \(.*\)/\1/' -e '/^minishell\$ $/d'
}

run_case() {
  local name="$1"; shift
  local script="$1"
  local tdir="$WORK_BASE/$name"
  rm -rf "$tdir" && mkdir -p "$tdir"

  pushd "$tdir" >/dev/null
  local b_out b_err b_ec m_out m_err m_ec
  b_out=$(mktemp); b_err=$(mktemp)
  bash -lc "$script" >"$b_out" 2>"$b_err" || true
  b_ec=$?

  m_out=$(mktemp); m_err=$(mktemp)
  cat >"$tdir/script.ms" <<'MS'
__SCRIPT_CONTENT_PLACEHOLDER__
MS
  python3 - "$tdir/script.ms" "$script" <<'PY'
import sys
path, content = sys.argv[1], sys.argv[2]
with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
PY
  "$MINI_SHELL" <"$tdir/script.ms" >"$m_out" 2>"$m_err" || true
  m_ec=$?
  local m_out_f m_err_f
  m_out_f=$(mktemp); m_err_f=$(mktemp)
  filter_ms_prompt <"$m_out" >"$m_out_f"
  filter_ms_prompt <"$m_err" >"$m_err_f"

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
  rm -f "$b_out" "$b_err" "$m_out" "$m_err" "$m_out_f" "$m_err_f"
  popd >/dev/null
  return $ok
}

ms() { cat; }

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

# Cases (no ';' or '&&')
run_or_count "echo_basic" "$(ms <<'SH'
echo hello world
SH
)"

run_or_count "echo_n_then_next_line" "$(ms <<'SH'
echo -n "no-newline"
printf "<END>\n"
SH
)"

run_or_count "pwd_cd_pwd" "$(ms <<'SH'
pwd
mkdir -p sub
cd sub
pwd
SH
)"

run_or_count "env_print" "$(ms <<'SH'
export FOO=bar
env | grep '^FOO='
SH
)"

run_or_count "missing_cmd" "$(ms <<'SH'
no_such_cmd arg1
SH
)"

echo
echo "Summary: PASS=$pass FAIL=$fail"
if [[ $fail -gt 0 ]]; then
  exit 1
fi
exit 0


