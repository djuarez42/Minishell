### MINISHELL — Complete Testing Guide (English)

This guide provides a thorough, copy‑pasteable test plan for minishell. Each section includes commented bash snippets and expected outcomes. Run interactive tests in a real TTY; non‑interactive tests can be piped.

Index
- 1. Compilation and build
- 2. Environment & PATH
- 3. Navigation and directories
- 4. Environment variables
- 5. Quotes and escaping
- 6. Pipes
- 7. Redirections
- 8. Builtin commands
- 9. Signals
- 10. Scripts and paths
- 11. Return values ($?)
- 12. Command history
- 13. Error handling
- 14. Global variables
- 15. Final verification checklist

---

## 1) Compilation and build

```bash
# Test 1: Normal build (should compile with no warnings/errors)
make

# Test 2: Dry run (see invoked commands only)
make -n

# Test 3: Full rebuild
make re

# Test 4: Clean artifacts
make clean && make fclean

# Test 5: Scan for globals (should be none except allowed)
grep -R "\bint\s\+g_" src/ || true
grep -R "\bchar\s\+g_" src/ || true
grep -R "^static\s\+[^ (].*=\|[^/]static\s\+[^ (].*=\|[^/]static\s\+[^ (].*{" src/ | grep -v "static .* (" || true
```

Expected:
- Clean build; Makefile targets work as expected
- No unapproved global variables

---

## 2) Environment & PATH

```bash
# Test 1: Standard commands should work via PATH
ls
wc --help | head -5
echo "123" | wc -c

# Test 2: Inspect PATH
echo "$PATH"

# Test 3: Remove PATH -> commands should be not found
unset PATH
ls || echo "ls: command not found"

# Test 4: Restore PATH (example; adapt to your system)
export PATH=/bin:/usr/bin
ls | head -3

# Test 5: Create a fake command and check PATH precedence
mkdir -p mybin
printf '%s\n' 'echo "fake ls"' > mybin/ls && chmod +x mybin/ls
export PATH="$(pwd)/mybin:$PATH"
ls  # should print: fake ls

# Test 6: Lower precedence of fake directory
export PATH="/bin:/usr/bin:$(pwd)/mybin"
ls  # should list directory again
```

Expected:
- With PATH unset: commands report "command not found"
- Precedence reflects PATH order

---

## 3) Navigation and directories

```bash
# Test 1: pwd parity with /bin/pwd
pwd; /bin/pwd

# Test 2: Basic navigation
pwd
mkdir -p testdir/subdir
cd testdir && pwd && ls
cd subdir && pwd
cd .. && pwd
cd . && pwd

# Test 3: Home directory
cd  # should go to $HOME
pwd | grep "$HOME"

# Test 4: Previous directory
cd /tmp && pwd
cd -   # should jump back and print previous path

# Test 5: Nonexistent path errors
cd does/not/exist || echo "cd: No such file or directory"

# Test 6: Permissions
mkdir -p no_access && chmod 000 no_access
cd no_access || echo "cd: Permission denied"
chmod 755 no_access && rmdir no_access
```

Expected:
- "cd" with no args -> HOME
- "cd -" -> previous directory and path printed
- Proper error messages for missing/permission denied

---

## 4) Environment variables

```bash
# Test 1: env prints only KEY=VALUE lines
env | head -10

# Test 2: export simple var
export TEST_VAR=hello
env | grep '^TEST_VAR=hello$'

# Test 3: export name only (should not create KEY=)
unset ONLY_NAME
export ONLY_NAME
env | grep '^ONLY_NAME=' && echo "unexpected" || echo "OK (no ONLY_NAME=)"

# Test 4: multiple vars
export A=1 B=2 C=3
env | grep -E '^(A|B|C)=' | sort

# Test 5: spaces via quotes
export STR="Hola mundo!"
env | grep '^STR=Hola mundo!$'

# Test 6: invalid names should error
export 123ABC=invalid 2>/dev/null || echo "export: not a valid identifier"
export VAR-NAME=bad   2>/dev/null || echo "export: not a valid identifier"

# Test 7: unset variables
export VAR1=one VAR2=two VAR3=three
env | grep -E 'VAR[123]=' | sort
unset VAR2
env | grep -E 'VAR[123]=' | sort  # VAR2 gone
unset VAR1 VAR3 NONEXISTENT  # no error for missing

# Test 8: HOME/OLDPWD presence
unset HOME; cd || echo "HOME not set"; export HOME="$PWD"
unset OLDPWD; cd - 2>/dev/null || echo "OLDPWD not set"
```

Expected:
- Name‑only export does not create KEY=
- Invalid names are rejected
- Unset silently ignores invalid or missing keys

---

## 5) Quotes and escaping

```bash
# Single quotes: literal
echo '$USER'
echo 'ls | wc -l'
echo 'hello > file'
echo ''
echo '!@#$%^&*()_+-=~'
echo 'hello$HOME'
echo uno 'dos tres' cuatro

# Double quotes: expansion allowed
echo "hola mundo con espacios"
echo "this is a string"
echo "cat lol.c | cat > lol.c"
echo " many   spaces  here "
echo hola "quoted world" end
echo "this is a \"test\" of quotes"
echo "value of \$PATH: $PATH" | head -1
echo "User: $USER, Home: $HOME"

# Mixed
echo "He said 'hello world'"
echo 'He said "hello world"'
echo "$USER says 'hello'"
echo '$USER says "hello"'
```

Expected:
- Single quotes do not expand variables
- Double quotes expand variables; spaces inside quotes are preserved

---

## 6) Pipes

```bash
# Simple pipes
echo "hola mundo" | grep hola
echo "foo" | wc -c
echo "a b c" | wc -w
ls | head -5

# Multiple pipes
cat /etc/passwd 2>/dev/null | grep root | wc -l
echo hola | cat | grep h | wc -l

# Pipe errors
ls file_that_does_not_exist | wc -l

# Syntax errors
printf '| ls\n' | ./minishell 2>&1 | grep -i 'syntax'
printf 'ls |\n' | ./minishell 2>&1 | grep -i 'syntax'
printf 'cat | | grep\n' | ./minishell 2>&1 | grep -i 'syntax'

# exit in pipeline does not exit shell
printf 'echo hello | exit\necho after\n' | ./minishell
```

Expected:
- Left‑to‑right pipeline behavior; syntax errors detected; exit inside pipe doesn’t exit shell

---

## 7) Redirections

```bash
# Output redirections
echo hola > out.txt && cat out.txt
echo line1 > log.txt
echo line2 >> log.txt
cat log.txt

# Input redirections
echo "this is a test" > test.txt
cat < test.txt

# Multiple inputs: last wins
echo file1 > file1.txt; echo file2 > file2.txt
cat < file1.txt < file2.txt

# Heredoc (end marker EOF)
cat << 'EOF'
line 1
line 2
EOF

# Heredoc to file
cat << 'FIN' > heredoc.txt
Hello
World
FIN
cat heredoc.txt

# Redirs + pipes
echo foo > file.txt
cat < file.txt | grep f > output.txt
cat output.txt

# Syntax errors
printf '>\n' | ./minishell 2>&1 | grep -i 'syntax'
printf 'echo hola >\n' | ./minishell 2>&1 | grep -i 'syntax'
printf 'cat <\n' | ./minishell 2>&1 | grep -i 'syntax'
printf 'cat <<\n' | ./minishell 2>&1 | grep -i 'syntax'
```

Expected:
- > overwrite; >> append; < read; << heredoc until marker; syntax errors reported

---

## 8) Builtin commands

```bash
# pwd
pwd
cd /tmp && pwd && cd ~ && pwd

# echo and flags
echo hello world
echo -n hello world; echo "<END>"
echo -nnnn hello
echo -na hello 2>/dev/null || echo "invalid flag handled"

# exit
printf 'exit 42\n' | ./minishell; echo $?
printf 'exit hello\n' | ./minishell 2>&1 | grep -i 'numeric'
printf 'exit 1 2\n' | ./minishell 2>&1 | grep -i 'too many'
```

Expected:
- pwd shows current dir; echo -n omits newline; exit handles arguments

---

## 9) Signals (interactive)

Run `./minishell` in a TTY and check:
- Ctrl‑C at empty prompt: prints a newline and shows a fresh prompt
- Ctrl‑C while `sleep 10`: interrupts sleep, returns to prompt
- Ctrl‑\ at empty prompt: does nothing
- Ctrl‑D at empty prompt: prints `exit` and terminates

---

## 10) Scripts and paths

```bash
# Simple script
echo 'echo "Hello from script!"' > my_script.sh
chmod +x my_script.sh
./my_script.sh

# Relative paths
mkdir -p testdir && cd testdir
../my_script.sh

# Complex paths
mkdir -p deep/nested/structure && cd deep/nested/structure
../../../my_script.sh
cd ../../../
././my_script.sh
```

Expected:
- Executables resolve properly via relative/complex paths

---

## 11) Return values ($?)

```bash
# Successes
true; echo $?
/bin/ls >/dev/null; echo $?

# Failures
/bin/ls does_not_exist 2>/dev/null; echo $?
cd /does/not/exist 2>/dev/null; echo $?
/bin/this-does-not-exist 2>/dev/null; echo $?

# Builtins
export TEST=value; echo $?
unset NONEXISTENT; echo $?
cd /nonexistent 2>/dev/null; echo $?

# Pipes and redirs
echo hello | grep hello >/dev/null; echo $?
echo hello | grep xyz   >/dev/null; echo $?
echo hello > /tmp/test.txt; echo $?
```

Expected:
- Success -> 0; command not found -> 127; typical errors -> non‑zero

---

## 12) Command history (interactive)

In TTY:
- Run a few commands, use Up/Down to navigate history
- After Ctrl‑C during a partially typed command, that partial line should not be saved in history
- Empty lines are not added to history

---

## 13) Error handling

```bash
# Nonexistent commands
nonexistentcommand 2>&1 | grep -i 'command not found'

# Unclosed quotes (example input)
printf 'echo "unclosed quote\n' | ./minishell 2>&1 | grep -i 'syntax\|quote'

# Random input
dsjcjrirbfjg 2>&1 | grep -i 'command not found' || true
12345         2>&1 | grep -i 'command not found' || true

# Empty/space-only input should do nothing
printf '\n   \n' | ./minishell >/dev/null
```

Expected:
- Clear error messages; robust handling of malformed input

---

## 14) Global variables & style

```bash
# Global search
grep -R "\bint\s\+g_" src/ || true
grep -R "\bchar\s\+g_" src/ || true
grep -R "volatile\|sig_atomic_t" src/ || true

# Norminette (if available locally)
norminette src/ include/
```

Expected:
- No forbidden globals; style passes 42 norms

---

## 15) Final verification checklist

- [ ] Builds cleanly without warnings
- [ ] PATH resolution correct (unset, precedence)
- [ ] All builtins work
- [ ] Quotes parsed correctly
- [ ] Pipes work in chains
- [ ] Redirections work (including heredoc)
- [ ] Signals handled (Ctrl‑C/\, Ctrl‑D)
- [ ] Variable expansion correct
- [ ] Return values correct
- [ ] History works interactively
- [ ] Error messages correct
- [ ] No leaks; memory freed
- [ ] Code style and no forbidden globals

### Quick composite test

```bash
echo "Complex test" | cat | grep test > result.txt && cat result.txt
export TEST=hello && echo "$TEST world" | wc -w
cd /tmp && pwd && cd - && pwd
unset PATH && export PATH=/bin && ls >/dev/null 2>&1 || echo "ok"
echo $?
```

Good luck at defense! 🚀


