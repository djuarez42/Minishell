## Test Failures Diagrams (2025-08-28)

### File: STD_OUT_failures_20250828_174816.txt

Key categories: tilde expansion missing, `$` edge cases (`$"`, `$'`, trailing `$`), `$USER` not expanding in double quotes due to heuristic, backslash-before-dollar parity, quoting concatenations.

```mermaid
graph TD
  classDef err fill:#ffe6e6,stroke:#ff4d4f,stroke-width:2px,color:#b30000;
  classDef warn fill:#fff7e6,stroke:#fa8c16,stroke-width:1px,color:#8c4a00;

  A["echo cd ~"] --> B["Tokenizer"]
  B --> C["Token: ~ (WORD)"]
  C --> D["expand_argv"]
  D --> E["tilde expansion"]
  E --> F["/home/user"]
  class E err

  G["echo $\"HOME\"$USER"] --> H["expand_variables"]
  H --> I{"$ followed by quote?"}
  I -- "yes" --> J["current: literal $"]
  class J err
  I -- "should" --> K["disappear (empty)"]

  L["echo """"""""$USER""""""""] --> M["expand_variables in dquotes"]
  M --> N["$USER expands"]
  class N err

  O["echo \\...$USER"] --> P["backslash-dollar handling"]
  P --> Q["current: always literal $ (1-escape only)"]
  class Q err
  P --> R["required: parity of backslashes decides expansion vs literal"]
```

---

### File: STD_OUT_failures_20250828_175159.txt

Key categories: syntax error handling for standalone/sequence of operators, tilde token as invalid command, pipeline edge cases, heredoc/append syntax forms.

```mermaid
graph TD
  classDef err fill:#ffe6e6,stroke:#ff4d4f,stroke-width:2px,color:#b30000;

  A[">" alone] --> B["Tokenizer/Parser"] --> C["should: syntax error"]
  class C err
  D["< < < < < <"] --> B
  E[">| echo wtf"] --> B
  F["~"] --> G["Tokenizer"] --> H["should: command not found or literal per rules"]
  class H err
```

---

### File: STD_OUT_failures_20250828_175330.txt

Key categories: redirection parsing with no spaces (operator adjacency), quote stripping for filenames, proper pipe behavior with redirections, heredoc behavior (quoted delimiters), printing of arguments mistakenly including redirs.

```mermaid
graph TD
  classDef err fill:#ffe6e6,stroke:#ff4d4f,stroke-width:2px,color:#b30000;

  A["cat <minishell.h>./outfiles/outfile"] --> B["Tokenizer"]
  B --> C["current: single WORD token"]
  class C err
  B --> D["should: '<' token, 'minishell.h' WORD, '>' token, filename WORD"]
  D --> E["Parser redirs list"]
  E --> F["expand_redirs + strip quotes"]

  G["/bin/echo 1 >/dev/null | grep 1"] --> H["Parser"]
  H --> I["current: treats '>/dev/null' as argv"]
  class I err
  H --> J["should: OUT redir before pipe"]

  K["cat << 'lim'"] --> L["Heredoc"]
  L --> M["current: missing reader"]
  class M err
  L --> N["should: prompt, collect until 'lim', no expansion"]
```

---

### File: STD_OUT_failures_20250828_175708.txt

Key categories: similar syntax error cases as previous, additional heredoc lines showing lack of implementation.

```mermaid
graph TD
  classDef err fill:#ffe6e6,stroke:#ff4d4f,stroke-width:2px,color:#b30000;

  A["<< lim / << EOF / << 'lim'"] --> B["Heredoc engine"]
  B --> C["current: prints '> ' only"]
  class C err
  B --> D["should: handle quoted/unquoted delimiter rules"]

  E["|, >>|, <| forms"] --> F["Syntax validator"]
  F --> G["should: report syntax error near unexpected token"]
  class G err
```


