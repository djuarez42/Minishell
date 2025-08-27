# Mixed Quote Handling Fix for Minishell

## Problem Description

The minishell was not correctly handling mixed quote combinations like `""'$USER'""`. In Bash, this should output `$USER` literally because the single quotes prevent variable expansion, but our minishell was expanding it to the actual username.

## Analysis

The issue occurs because:
1. The input `""'$USER'""` consists of empty double quotes, followed by single-quoted `$USER`, followed by empty double quotes
2. In Bash, single quotes preserve everything literally, including the `$` character
3. Our tokenizer correctly parses this as a single token, but the expansion logic was not respecting the single-quote context within the mixed quotes

## Fix Implementation

We created a specialized function `expand_mixed_quotes()` in `src/executor/expand_mixed_quotes.c` that:

1. Parses the input character by character
2. Identifies quote boundaries (both single and double quotes)
3. For single-quoted sections: preserves content literally (no variable expansion)
4. For double-quoted sections: allows variable expansion
5. For unquoted sections: allows variable expansion

The function is called from `expand_argv()` when mixed quote patterns are detected.

## Current Status

This is a complex issue that requires careful handling of quote contexts during tokenization and expansion. The fix is implemented but may need further refinement to handle all edge cases correctly.

## Test Cases

```bash
echo ""'$USER'""          # Should output: $USER
echo '"'$USER'"'          # Should output: "username"
echo ''''''''''$USER''''''''''  # Should output: $USER
```

The goal is to match Bash's behavior exactly for these complex quote combinations.
