## Expansion, Tokenizer, and Parser Alignment Plan

### Scope

- Compare current implementation under `src/lexer/*`, `src/executor/*` with the behavior described in the attached `EXPANSION.md` doc.
- Identify mismatches and propose minimal, safe edits to align behavior.
- Provide targeted code edits (snippets) for clarity.

### Reference order (from EXPANSION.md)

- tokenization → syntax checks → expansion → command building

---

## Findings vs Spec

### 1) Tokenization and quote handling

- Current: `reconstruct_words` keeps quoted segments intact and tracks `t_quote_type` through `last_quote` updated in `handle_quoted_part`.
- Current: `extract_quoted_segment` returns content without outer quotes, ensuring single/double-quoted parts remain as one token piece.
- Current: `token_len` jumps over quoted text; operators are separated.
- Spec parity: OK for keeping quoted segments and word boundaries.

Gaps:
- `remove_quotes` only strips surrounding double quotes, not single quotes. Spec suggests filenames in redirections should have both quote characters removed after expansion (quotes are not part of the path).

Action:
- Adjust redirection filename handling to strip both `'` and `"` characters after expansion (see Code Edits A).

### 2) Variable expansion semantics

- Current: `expand_variables` expands `$` when not in single quotes; supports `\$` as literal `$` (backslash), which is not in the reference spec but can be tolerated if not harmful.
- Current special cases: `$?` via `expand_exit_status`; `$` followed by digit/alpha handled via `skip_variable_name`; `$` followed by `\0` returns literal `$` (see `handle_special_dollar`) — Spec expects trailing `$` to disappear (empty), not a literal `$`.
- Current: `$"` returns literal `$` and then double-quoted text handled separately — Spec expects `$"` to make the `$` disappear and then double-quoted content be concatenated. Effective arg equals the double-quoted content without the `$`.
- Current: Invalid name start after `$` returns literal `$`. Spec says: if followed by invalid name-start and not a quote, expand to literal `$` — matches.
- Current: `$'$...` case — code treats `$'` as empty via `handle_special_dollar`? Currently returns NULL for `$'` then falls into variable name branch returning `$` literal if no valid name. Spec expects `$'` → empty for `$` and then single-quoted text literal — final is the single-quoted content.

Required adjustments:
- Trailing `$` at end of token must disappear (empty), not output `$`. (Code Edits B)
- `$"` and `$'` should cause the `$` to disappear (empty contribution), then the quote handling emits the quoted content appropriately. (Code Edits B)

Note:
- `$<digit>` should expand env var named with that digit; current logic via `skip_variable_name` only accepts alpha or `_` as start. Must allow digit as first char after `$` to capture a single digit name. (Code Edits C)

### 3) Double quotes vs single quotes

- Current: `expand_variables` respects `QUOTE_SINGLE` to suppress expansion and expands inside `QUOTE_DOUBLE` and none.
- Mixed quotes: `expand_mixed_quotes` attempts to re-walk input; but expansion pipeline already tokenizes and tracks quotes, so a simpler approach is to expand per argv element using its `t_quote_type`, mixing is already concatenated during tokenization. Keep `expand_mixed_quotes` unused or ensure it doesn’t conflict.

Action:
- Prefer consistent expansion via `expand_argv` and `expand_redirs`. Remove special-case `$USER` checks (they leak test heuristics) and rely on rules. (Code Edits D)

### 4) Tilde expansion

- Spec mentions tilde expansion for tokens starting with `~` or `~/`. Project currently has no tilde expansion.

Action:
- Add optional tilde expansion: If token begins with `~` or `~/`, replace leading `~` with `$HOME`. (Code Edits E)

### 5) Redirection filenames quote stripping

- Current: redirs expanded via `expand_redirs` but filenames are not explicitly stripping quotes after expansion.
- Spec: quotes removed for redirection filenames after expansion.

Action:
- Apply quote removal on `redir->file` post-expansion, removing both `'` and `"` characters. (Code Edits A)

---

## Code Edits

### Code Edits A: Strip quotes in redirection filenames

Apply after `expanded` is produced in `expand_redirs`:

```c
// in expand_variables_utils2.c, inside expand_redirs loop after assigning expanded
char *stripped = remove_all_quotes(expanded); // new helper strips both ' and "
if (!stripped) { free(expanded); return (-1); }
free(expanded);
redir->file = stripped;
```

Add helper in a lexer/executor-shared place:

```c
// remove_all_quotes: remove all quote characters from a string
char *remove_all_quotes(const char *s) {
    size_t len = ft_strlen(s);
    char *out = malloc(len + 1);
    size_t j = 0;
    if (!out) return NULL;
    for (size_t i = 0; i < len; i++) {
        if (s[i] != '\'' && s[i] != '"') out[j++] = s[i];
    }
    out[j] = '\0';
    return out;
}
```

### Code Edits B: `$` edge cases (`$"`, `$'`, trailing `$`)

In `handle_special_dollar` and `handle_dollar`:

```c
// handle_special_dollar: when input[*i+1] == '\0' → return empty string, not "$"
if (!input[start]) { *i = *i + 1; return ft_strdup(""); }

// In handle_dollar, before variable name parsing
if (input[start] == '"' || input[start] == '\'') {
    // $ followed immediately by a quote → disappears
    *i = *i + 1; // consume only '$'
    return ft_strdup("");
}
```

### Code Edits C: `$<digit>` support

Extend `skip_variable_name` or add a fast path to return length 1 for leading digit after `$`:

```c
// in handle_dollar, after start is computed
if (ft_isdigit((unsigned char)input[start])) {
    name = ft_substr(input, start, 1);
    *i = start + 1;
    return expand_env_var(name, envp);
}
```

Or generalize `is_var_start` to accept digits only when used with a separate rule (keeping env identifiers strict elsewhere).

### Code Edits D: Remove heuristic `$USER` skip in `expand_argv`

Replace the special-case block with a straight call to `expand_variables` for all non-single-quoted argv elements:

```c
// in expand_argv, remove the conditional that checks for $USER pattern
expanded = expand_variables(argv[j], envp, state, quote_type);
```

### Code Edits E: Tilde expansion (token-level)

Perform before `$` expansion in argv and redirs for tokens that begin with `~`:

```c
static char *expand_tilde_prefix(const char *s, char **envp) {
    if (!s || s[0] != '~') return ft_strdup(s);
    if (s[1] != '\0' && s[1] != '/') return ft_strdup(s); // Only ~ or ~/ supported
    const char *home = env_get_value(envp, "HOME");
    if (!home) home = "";
    char *suffix = ft_strdup(s + 1); // keep possible '/...'
    char *res = ft_strjoin(home, suffix);
    free(suffix);
    return res;
}

// In expand_argv for each argv[j] before expand_variables
char *tilde = expand_tilde_prefix(argv[j], envp);
char *expanded = expand_variables(tilde, envp, state, quote_type);
free(tilde);
```

Note: Skip tilde expansion for single-quoted args (treat literally) to align with typical shells where quotes can suppress expansions.

---

## Parser considerations

- Keep `t_quote_type` per argv to decide if expansion should occur. For mixed segments in a single token, current tokenizer concatenates prior to expansion; `$` semantics are handled during expansion.
- Redirections: Ensure `parse_redirections` sets `quoted` if needed for heredoc delimiter behavior (quoted delimiter disables variable expansion in heredoc). If not present, consider adding a flag from token quote type.

---

## Minimal Edit Checklist

- Implement `$` edge-case fixes (B) and digit var support (C).
- Remove `$USER` heuristic (D).
- Add optional tilde expansion (E).
- Strip quotes in redirection filenames (A).

---

## Test vectors (from EXPANSION.md)

Run these after changes to verify behavior:

- `/bin/echo $"HOME"$USER` → `HOME<USER>`
- `/bin/echo $"HOM"E$USER` → `HOME<USER>`
- `/bin/echo $'HOME'` → `HOME`
- `/bin/echo ""'$USER'""` → `$USER`
- `echo ~ ~/bin` → `<HOME> <HOME>/bin`
- `echo $` → empty line
- Redirection: `echo x > "out.txt"` creates file `out.txt`

---

## Appendix: Exact edit locations

- `src/executor/expand_variables_utils2.c`
  - `handle_special_dollar`: trailing `$` case → return empty
  - `handle_dollar`: handle `$"` and `$'` to disappear; add `$<digit>` path
  - `expand_redirs`: after expansion, strip quotes via `remove_all_quotes`

- `src/executor/expand_variables_utils.c`
  - Optionally add `remove_all_quotes` helper (or in a shared utils file)

- `src/executor/expand_variables.c`
  - `expand_argv`: remove `$USER` heuristic; add tilde expansion before `$`

---

This plan keeps changes localized, aligns with the provided semantics, and avoids invasive refactors.

---

## Expansion in minishell

### Overview

- **Order**: tokenization → syntax checks → expansion → command building.
- Implemented in reference doc as `src/expand/expand.c` and `src/expand/expand2.c`.
- Expansion runs on `T_WORD` tokens; redirection filenames have quotes removed after expansion.

Mapping to this project:
- Tokenization: `src/lexer/tokenizer.c`, `tokenizer_utils_2.c`, `token_lexer_utils.c`, `lexer_utils_3.c`.
- Expansion: `src/executor/expand_variables.c`, `expand_variables_utils.c`, `expand_variables_utils2.c`.
- Redirection filenames: `expand_redirs` with planned strip via `remove_all_quotes` (Code Edits A).

### Tokenization basics (context)

- Quoted segments are kept inside a single token. The lexer jumps over the matching closing quote.
  - Equivalent here: `extract_quoted_segment`, `token_len`, `handle_quoted_part`.
- Delimiters that end an unquoted chunk: whitespace, `|`, `<`, `>`, `$`, single-quote, double-quote.

### Quote semantics

- **Single quotes `'...'`**: no expansion inside. The outer quotes are not kept; only inner literal text is appended to the result.
- **Double quotes `"..."`**: `$`-expansion is performed inside; the quotes themselves are not kept.
- **Unquoted text**: appended literally until a delimiter is met; `$` triggers a variable expansion.

### Variable expansion rules

- General form: `$NAME` where `NAME` is `[A-Za-z0-9_]+`.
  - Looks up `NAME` in the environment; if unset, expands to an empty string.
- Special parameters:
  - **`$?`** → last command exit code (as decimal string).
  - **`$<digit>`** (e.g., `$1`) → treated as env var named by that single digit; usually expands to empty (not positional params).
- Edge handling after `$`:
  - If followed by an invalid name-start and not a quote, expands to a literal `$`.
  - If followed immediately by a quote (e.g., `$"` or `$'`), expands to an empty string (the `$` disappears).
  - Trailing `$` at end of token expands to empty (the `$` disappears).

Project changes addressing this section:
- Implemented/adjusted in `handle_special_dollar` and `handle_dollar` (Code Edits B, C).

### Tilde expansion

- If a token begins with `~` or `~/`, the leading `~` is replaced with `$HOME` once, then the rest is concatenated.
  - Examples: `~` → `/home/user`; `~/bin` → `/home/user/bin`.

Project changes: Add `expand_tilde_prefix` and call before `$`-expansion for non-single-quoted args (Code Edits E).

### Redirection filenames

- For tokens classified as filenames (`IN_FILE` / `OUT_FILE`), all quote characters are removed after expansion.
  - This mirrors shell behavior where quotes affect parsing but are not part of the path string.

Project changes: Strip with `remove_all_quotes` in `expand_redirs` (Code Edits A).

### Concatenation behavior

- A single token can mix quoted and unquoted segments; the expanded pieces are concatenated without inserting extra spaces.
  - Example: `$USER"'"` → `<username>'` (username plus a literal single-quote).

Already satisfied: `reconstruct_words` preserves segments; `expand_variables` concatenates pieces.

### Unsupported features (by design here)

- No globbing (`*`, `?`), no brace expansion, no command substitution, no backslash escaping. Certain characters like `\\`, `;`, `&`, `(`, `)` are rejected as syntax errors when unquoted.

Note: Current backslash handling for `\\$` is permissive; acceptable as a literal `$` passthrough.

### Worked examples

- Input: `echo 'abc'"def"ghi`
  - Single quotes: `abc` (literal)
  - Double quotes: `def` (literal, with `$`-expansion if present)
  - Unquoted: `ghi`
  - Arg becomes: `abcdefghi`

- Input: `echo '"' $USER"'"`
  - Arg1: `\'"'` → `"` (a double-quote character)
  - Arg2: `$USER"'"` → `<username>'`
  - Printed: `" <username>'`

- Input: `echo ~ ~/bin`
  - Expands to: `/home/user /home/user/bin` (using `$HOME`).

- Input: `echo $1 X$?Y $Z`
  - `$1` → env var `1` (likely empty)
  - `X$?Y` → `X<exit_code>Y`
  - `$Z` → value of `Z` or empty

### Key implementation points (files)

- Reference functions: `ft_get_expanded_str`, `ft_grab_next_str`, `ft_expandizer`, `ft_next_str_in_double_quotes`, `ft_get_expand`, `ft_grab_str`, `get_token_size`.
- Project equivalents: `expand_variables`, `handle_dollar`, `extract_plain_text`, `expand_argv`, `expand_redirs`, `extract_quoted_segment`, `token_len`.

### Practical tips

- To inject a literal quote into an argument, wrap it in the other quote type: `\'"'` or `"'"`.
- To concatenate expansions and literals without spaces, keep them in the same token: `prefix_$VAR_suffix`.

### Step-by-step tutorial

1) Echo a literal double-quote and username followed by a single-quote

- Command:
  - `echo '"' $USER"'"`
- Steps:
  1. Tokenize → `echo` | `\'"'` | `$USER"'"`
  2. Expand `\'"'` → `"`
  3. Expand `$USER` → your username
  4. Inside `"'"`, no `$` occurs; result is `'`
  5. Concatenate → args: `"` and `<username>'`
- Output:
  - `" <username>'`

2) Mix single, double, and unquoted text in one token

- Command:
  - `echo 'abc'"def"ghi`
- Steps:
  1. Tokenize into `echo` | `\'abc'"def"ghi`
  2. Inside `\'abc'` → literal `abc`
  3. Inside `"def"` → literal `def` (with `$`-expansion if present)
  4. Unquoted `ghi` → literal
  5. Concatenate → `abcdefghi`
- Output:
  - `abcdefghi`

3) Exit code interpolation

- Command:
  - `false ; echo X$?Y`
- Steps:
  1. `false` sets exit code (commonly 1)
  2. Tokenize `echo` | `X$?Y`
  3. Expand `$?` to last exit code → `1`
  4. Concatenate → `X1Y`
- Output:
  - `X1Y`

4) Tilde at token start

- Command:
  - `echo ~ ~/bin`
- Steps:
  1. For tokens starting with `~` or `~/`, replace `~` with `$HOME`
  2. If `$HOME` is `/home/user` → `/home/user` and `/home/user/bin`
- Output:
  - `/home/user /home/user/bin`

5) Edge cases after `$`

- Commands and results (assuming unset `FOO`):
  - `echo $` → empty (the trailing `$` disappears)
  - `echo $"abc"` → empty for `$` part, then `abc`
  - `echo $'abc'` → empty for `$` part, then `abc`
  - `echo $9` → expands var `9` (usually empty)
  - `echo $FOO-bar` → empty for `$FOO`, then `-bar`

6) Redirection filenames: quotes are removed

- Command:
  - `echo data > "out.txt"`
- Steps:
  1. Tokenize `>` then `"out.txt"` (filename token)
  2. Expansion runs; filename tokens then have quotes stripped
  3. File actually opened as `out.txt`

7) Unsupported metacharacters must be quoted

- Command:
  - `echo a;b` → syntax error (unquoted `;` is rejected)
- Use quotes to pass them literally:
  - `echo 'a;b'` → prints `a;b`

### Parsing examples (advanced cases)

Below, `<USER>` means the value of `$USER` and `<HOME>` means the value of `$HOME`.

1) `/bin/echo $"HOME"$USER`

- Tokens: `/bin/echo` | `$"HOME"$USER`
- Expansion:
  - `$"` → empty (the `$` disappears)
  - `"HOME"` → `HOME`
  - `$USER` → `<USER>`
- Resulting argv: `[/bin/echo, HOME<USER>]`
- Output: `HOME<USER>`

2) `/bin/echo $"HOM"E$USER`

- Tokens: `/bin/echo` | `$"HOM"E$USER`
- Expansion:
  - `$"` → empty
  - `"HOM"` → `HOM`
  - `E` → literal `E`
  - `$USER` → `<USER>`
- Resulting argv: `[/bin/echo, HOME<USER>]`
- Output: `HOME<USER>`

3) `/bin/echo $"'HOM'E"$USER`

- Tokens: `/bin/echo` | `$"'HOM'E"$USER`
- Expansion:
  - `$"` → empty
  - `"'HOM'E"` → `'<HOM>'E` (single quotes literal inside double quotes)
  - `$USER` → `<USER>`
- Resulting argv: `[/bin/echo, 'HOM'E<USER>]`
- Output: `'HOM'E<USER>`

4) `/bin/echo $'HOM'E$USER`

- Tokens: `/bin/echo` | `$'HOM'E$USER`
- Expansion:
  - `$'` → empty
  - `'HOM'` → `HOM`
  - `E` → literal `E`
  - `$USER` → `<USER>`
- Resulting argv: `[/bin/echo, HOME<USER>]`
- Output: `HOME<USER>`

5) `/bin/echo $"HOME"`

- Tokens: `/bin/echo` | `$"HOME"`
- Expansion:
  - `$"` → empty
  - `"HOME"` → `HOME`
- Resulting argv: `[/bin/echo, HOME]`
- Output: `HOME`

6) `/bin/echo $'HOME'`

- Tokens: `/bin/echo` | `$'HOME'`
- Expansion:
  - `$'` → empty
  - `'HOME'` → `HOME`
- Resulting argv: `[/bin/echo, HOME]`
- Output: `HOME`

7) `/bin/echo ""'$USER'""`

- Tokens: `/bin/echo` | `""'$USER'""`
- Expansion:
  - `""` → empty
  - `'$USER'` → `$USER` (single quotes suppress expansion)
  - `""` → empty
- Resulting argv: `[/bin/echo, $USER]`
- Output: `$USER`

8) `/bin/echo '\"'$USER'\"'`

- Tokens: `/bin/echo` | `\'"'$USER'"'`
- Expansion:
  - `\'"'` → `"`
  - `$USER` → `<USER>`
  - `\'"'` → `"`
- Resulting argv: `[/bin/echo, \"<USER>\"]`
- Output: `"<USER>"`

9) `/bin/echo "''$USER''"`

- Tokens: `/bin/echo` | `"''$USER''"`
- Expansion (inside the double quotes):
  - `''` → two literal single-quote characters
  - `$USER` → `<USER>`
  - `''` → two literal single-quote characters
- Resulting argv: `[/bin/echo, ''<USER>'']`
- Output: `''<USER>''`

10) `/bin/echo "'"'$USER'"'"`

- Tokens: `/bin/echo` | `"'"'$USER'"'"`
- Expansion:
  - `"'"` → `'` (a single-quote character)
  - `$USER` → `<USER>`
  - `"'"` → `'`
- Resulting argv: `[/bin/echo, '<USER>']`
- Output: `'<USER>'`

Coverage status in this project after edits:
- 1–2, 5–6, 8–10: covered by Code Edits B (edge `$` cases) and removal of `$USER` heuristic.
- 3, 7, 9: handled by double-quote processing with literal single quotes and `$` expansion inside `expand_variables` when `quote == QUOTE_DOUBLE`.
- Tilde cases covered by Code Edits E.

---

## Contributor split: A and B

### Contributor A (Tokenizer/Parser/Redirs/Heredoc)
- Tokenizer: stop plain spans on operators too.
  - Edit `handle_plain_text` to break on `|`, `<`, `>`.
  - When encountering an operator at `input[i]`, emit it as its own token using `operator_len` (1 or 2 chars).
- Tokenizer: optionally break on `$` as delimiter or leave to expansion; keep current approach but ensure expansion covers all `$` cases (Contributor B).
- Parser: robust syntax validation.
  - Detect standalone operators (`>`, `<`, `<<`, `|`) and invalid sequences (`>|`, `<|`, `>>|`, leading/trailing `|`).
  - Ensure redirection binds to nearest following WORD token.
- Redirections: quote stripping for filenames after expansion.
  - Use `remove_all_quotes` in `expand_redirs`.
- Heredoc engine:
  - Parse `<<` with delimiter token; set `quoted` flag if delimiter token was quoted.
  - Implement line reader until delimiter; perform expansion only if not quoted.
  - Wire with `t_heredoc_args` utilities.
- Operator adjacency (no spaces): support inputs like `cat <in>out` → tokens `<` `in` `>` `out`.

### Contributor B (Expansion semantics)
- Remove `$USER` heuristic; expand in double-quoted and unquoted contexts per rules.
- `$` edge cases:
  - `$"` and `$'` → `$` disappears (empty).
  - Trailing `$` → empty.
  - `$<digit>` → expand single-digit var name.
  - Invalid name-start (and not quote) → literal `$`.
- Backslash before `$` (unquoted and double-quoted): implement parity logic.
  - Count consecutive backslashes immediately before `$`.
  - Even count: output half the backslashes, then expand variable.
  - Odd count: output half the backslashes, then literal `$` (no expansion).
- Tilde expansion at token start (non-single-quoted): `~` or `~/` → `$HOME` prefix.
- Double quotes: allow `$` expansion inside; concatenate literal segments.
- Redirection filenames: after expansion, strip both `'` and `"` from path string.

---

## Updated step-by-step tutorial (focused on failing cases)

1) Tilde at token start in a normal word
- Command: `echo cd ~`
- Steps: tokenize → `echo` | `cd` | `~`; tilde expands to `$HOME`; print `cd <HOME>`.

2) `$` followed by quotes disappears
- Command: `/bin/echo $"HOME"$USER`
- Steps: `$"` → empty; `"HOME"` → `HOME`; `$USER` expands; output `HOME<USER>`.

3) `$` followed by single quotes disappears; inside single quotes no expansion
- Command: `/bin/echo $'HOME'`
- Steps: `$'` → empty; `'HOME'` → `HOME`; output `HOME`.

4) Many quotes around `$USER` inside double quotes
- Command: `echo """"""""$USER""""""""`
- Steps: empty chunks from `""`; `$USER` expands; output `<USER>`.

5) Mixed quotes producing literal quotes around expanded content
- Command: `echo '"' $USER"'"`
- Steps: `'"'` → `"`; `$USER` expands; `"'"` → `'`; output `" <USER>'`.

6) Backslashes before `$` control expansion
- Commands/examples:
  - `echo \\$USER` → `\<USER>` (even backslashes → expand)
  - `echo \\\$USER` → `\$USER` (odd backslashes → literal `$`)

7) Redirection filenames strip quotes
- Command: `echo data > "out.txt"`
- Steps: `>` token; filename token expands then quotes removed; file opened as `out.txt`.

8) Operator adjacency without spaces
- Command: `cat <minishell.h>./outfiles/outfile`
- Steps: tokenize `<` | `minishell.h` | `>` | `./outfiles/outfile`; parser builds redirs; OK after tokenizer fix.

9) Heredoc with quoted delimiter disables expansion
- Command: `cat << 'lim'`
- Steps: collect lines until `lim`; no expansion of `$VAR` inside body.

10) Syntax errors on isolated/invalid operators
- Commands: `>`, `<`, `<<`, `|`, `>>|`, `<|`, `| |` …
- Steps: tokenizer emits operators; parser reports “syntax error near unexpected token”.


