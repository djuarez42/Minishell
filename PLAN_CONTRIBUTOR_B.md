## Contributor B Plan: Expansion Semantics and Tilde

### Goals
- Fix `$` edge cases: `$"`, `$'`, trailing `$`, `$<digit>`, invalid name start.
- Remove `$USER` heuristic; always expand in double quotes and unquoted context per rules.
- Implement backslash-dollar parity handling in unquoted and double-quoted text.
- Implement tilde expansion at token start (`~`, `~/`) for non-single-quoted tokens.
- Ensure redirection filenames have quotes stripped after expansion.

### 1) Remove `$USER` heuristic in `expand_argv`

File: `src/executor/expand_variables_utils2.c`

Change:

```c
// Replace the special-case block with a direct call
expanded = expand_variables(argv[j], envp, state, quote_type);
```

### 2) `$` edge cases in `handle_special_dollar` / `handle_dollar`

File: `src/executor/expand_variables_utils2.c`

Cases:
- Trailing `$` → empty.
- `$"` and `$'` → `$` disappears (empty), then quote handling continues by tokenizer context.
- `$<digit>` → expand single digit as var name.
- Invalid start and not a quote → literal `$` (already handled).

Code edits (snippets):

```c
char	*handle_special_dollar(const char *input, int *i, t_exec_state *state)
{
    int start = *i + 1;
    if (!input[start]) { *i = *i + 1; return ft_strdup(""); }
    if (input[start] == '?') { *i = start + 1; return expand_exit_status(state); }
    if (input[start] == '$') { *i = start + 1; return ft_strdup("$"); }
    return NULL;
}

char	*handle_dollar(const char *input, int *i, char **envp, t_exec_state *state)
{
    int start = *i + 1;
    char *res;
    res = handle_special_dollar(input, i, state);
    if (res) return res;

    if (input[start] == '"' || input[start] == '\'')
    { *i = *i + 1; return ft_strdup(""); }

    if (ft_isdigit((unsigned char)input[start]))
    {
        char name[2] = { input[start], '\0' };
        *i = start + 1;
        return expand_env_var(name, envp);
    }

    int len = skip_variable_name(input + start);
    if (len == 0) { *i = *i + 1; return ft_strdup("$"); }
    char *name = ft_substr(input, start, len);
    res = expand_env_var(name, envp);
    free(name);
    *i = start + len;
    return res;
}
```

### 3) Backslash-dollar parity

File: `src/executor/expand_variables.c` and/or `expand_variables_utils2.c`

Rule:
- Count consecutive backslashes immediately before `$`.
- If even: output half the backslashes, then treat `$` as expansion.
- If odd: output half the backslashes, then output literal `$` and do not expand.

Snippet (inside `expand_variables` loop before handling `$`):

```c
int bs = 0; int k = i - 1;
while (k >= 0 && input[k] == '\\') { bs++; k--; }
if (bs % 2 == 1)
{
    // odd: literal dollar
    // append bs/2 backslashes (already present in tmp), then a '$'
    piece = ft_strdup("$");
    tmp = str_append(tmp, piece); free(piece);
    i++;
    continue;
}
// even: expansion proceeds
```

Careful: When accumulating plain text, avoid swallowing backslashes that affect `$` parity. Consider adjusting `extract_plain_text` to stop before sequences of backslashes followed by `$`.

### 4) Tilde expansion at token start

File: `src/executor/expand_variables_utils2.c`

Add helper and call from `expand_argv` before `$` expansion for non-single-quoted args:

```c
static char *expand_tilde_prefix(const char *s, char **envp)
{
    if (!s || s[0] != '~') return ft_strdup(s);
    if (s[1] && s[1] != '/') return ft_strdup(s);
    const char *home = env_get_value(envp, "HOME");
    if (!home) home = "";
    char *suffix = ft_strdup(s + 1);
    char *res = ft_strjoin(home, suffix);
    free(suffix);
    return res;
}

// in expand_argv, before expand_variables
if (quote_type != QUOTE_SINGLE)
{
    char *tilde = expand_tilde_prefix(argv[j], envp);
    expanded = expand_variables(tilde, envp, state, quote_type);
    free(tilde);
}
else
{
    expanded = ft_strdup(argv[j]);
}
```

### 5) Redirection filenames: quote stripping after expansion

File: `src/executor/expand_variables_utils2.c` (in `expand_redirs`)

```c
expanded = expand_variables(redir->file, envp, state, QUOTE_NONE);
char *stripped = remove_all_quotes(expanded);
free(expanded);
redir->file = stripped;
```

Helper (shared):

```c
char *remove_all_quotes(const char *s)
{
    size_t len = ft_strlen(s), j = 0;
    char *out = malloc(len + 1);
    if (!out) return NULL;
    for (size_t i = 0; i < len; i++)
        if (s[i] != '\'' && s[i] != '"') out[j++] = s[i];
    out[j] = '\0';
    return out;
}
```

### 6) Tests covered by B
- `$"HOME"$USER`, `$"HOM"E$USER`, `$"HOME"` → `$` disappears then expand.
- `""""""""$USER""""""""` → `$USER` expands inside double quotes.
- `\$USER` sequences honor backslash parity.
- `$1`, `$9`: expand single-digit env vars.
- `echo cd ~` → tilde expands to `$HOME` prefix.

### 7) Acceptance criteria
- All variable expansion cases from `EXPANSION.md` pass.
- Failing tests around quotes, `$` edge cases, and tilde now match bash outputs.
- No heuristic exceptions remain; behavior is rule-driven.


