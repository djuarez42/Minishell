## Contributor A Plan: Tokenizer, Parser, Redirections, Heredoc

### Goals
- Split operators correctly even without spaces (e.g., `cat <in>out`).
- Improve syntax error detection for invalid operator sequences and isolated operators.
- Ensure redirection filenames are parsed as separate tokens and quotes removed after expansion.
- Implement heredoc parsing and reading; support quoted delimiter (no expansion) vs unquoted (expand).

### 1) Tokenizer: break on operators and emit operator tokens

Files: `src/lexer/lexer_utils_4.c`, `src/lexer/token_lexer_utils.c`

Changes:
- Update `handle_plain_text` to stop at `|`, `<`, `>`, and quotes.
- When the main loop sees an operator, emit it as its own token using `operator_len()` and advance accordingly.

Code edits (snippets):

```c
// lexer_utils_4.c
char	*handle_plain_text(const char *input, int *i, char *tmp)
{
    int		start;
    char	*segment;

    start = *i;
    while (input[*i]
        && !is_quote(input[*i])
        && !ft_isspace((unsigned char)input[*i])
        && !is_operator(input[*i]))
        (*i)++;
    if (*i > start)
    {
        segment = ft_substr(input, start, *i - start);
        tmp = str_append(tmp, segment);
        free(segment);
    }
    return (tmp);
}
```

```c
// token_lexer_utils.c (in process loop)
if (is_operator(input[i]))
{
    int len = operator_len(&input[i]);
    char *op = ft_substr(input, i, len);
    add_token(tokens, &tok_i, &op);
    quotes_out[tok_i - 1] = QUOTE_NONE;
    i += len;
    continue;
}
```

### 2) Parser: robust syntax checks for operators

Files: `src/parser/parser.c` (or relevant), `include/parser.h`

Checks to add:
- Leading/trailing `|` → syntax error.
- Isolated `>`, `<`, `<<` without following WORD → syntax error.
- Sequences like `>|`, `<|`, `>>|` → syntax error near unexpected token.

Pseudo-code:

```c
if (cur->type == TOKEN_PIPE && (prev == NULL || next == NULL || next->type == TOKEN_PIPE))
    return syntax_error("|");
if ((cur->type == TOKEN_REDIRECT_IN || cur->type == TOKEN_REDIRECT_OUT
     || cur->type == TOKEN_HEREDOC || cur->type == TOKEN_APPEND)
    && (next == NULL || next->type != TOKEN_WORD))
    return syntax_error(token_type_str(cur->type));
```

### 3) Redirection filenames: separation and quote stripping

Ensure parser attaches the next WORD as filename even when tokens are adjacent due to step 1.

After expansion, strip quotes from filenames (Contributor B will provide `remove_all_quotes`; use it here):

```c
// after expanding redir->file
char *stripped = remove_all_quotes(redir->file);
free(redir->file);
redir->file = stripped;
```

### 4) Heredoc parsing and reading

Parsing:
- When seeing `TOKEN_HEREDOC` (`<<`), read next token as delimiter (WORD).
- Set `redir->type` to heredoc and set `redir->quoted = is_quoted(delim_token->value)` based on original token quote type.

Execution:
- Implement a heredoc reader that:
  - Prompts ("> ") and reads lines until the delimiter line.
  - If `quoted == false`: expand variables in lines (Contributor B logic), else keep literal.
  - Writes to a temporary file and sets stdin redirection for the command.

Code skeleton:

```c
int handle_redirections_heredoc(const char *delimiter, bool quoted, char **envp, t_heredoc_args *args)
{
    // open temp file, loop reading lines, compare to delimiter, expand if !quoted
    // write to temp, set args->fd, return 0/-1
}
```

### 5) Tests covered by A
- Operator adjacency: `cat <minishell.h>./out`
- Syntax errors: bare `>`, `<`, `<<`, `|`, sequences like `>>|`, `<|`, leading/trailing `|`.
- Redirs with quotes: `cat <"file with spaces"` (quote stripping after expansion).
- Heredoc forms: `<< lim`, `<< 'lim'`, `<< "lim"` (quoted disables expansion).

### 6) Acceptance criteria
- Tokenizer splits all operators correctly with or without spaces.
- Parser rejects invalid operator placements with clear error messages.
- Redirection filenames are separate tokens; quotes not present when opening files.
- Heredoc behaves per bash-like rules for quoted/unquoted delimiters.


