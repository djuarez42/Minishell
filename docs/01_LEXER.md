# Lexer & Tokenizer Component

## Overview

The lexer and tokenizer are responsible for breaking down the input string into meaningful tokens that can be processed by the parser. This component handles the initial processing of the command line input.

## Example: Processing `echo $'USER'`

### Input String

```
echo $'USER'
```

### Lexer Flow

1. The input string "echo $'USER'" is passed to `tokenize_input()`
2. `tokenize_input()` calls `parse_fragments()` to break the string into fragments
3. The fragments are then combined into tokens

### Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `tokenize_input()` | Main entry point for lexical analysis | `tokens = tokenize_input("echo $'USER'")` |
| `parse_fragments()` | Breaks input into fragments | `fragments = parse_fragments("echo $'USER'")` |
| `new_fragment()` | Creates a fragment structure | `frag = new_fragment("echo", 4, QUOTE_NONE)` |
| `append_fragment()` | Adds fragment to list | `append_fragment(&head, frag)` |
| `create_token()` | Creates a token from fragments | `token = create_token(frag, TOKEN_WORD)` |
| `append_token()` | Adds token to list | `append_token(&tokens, token)` |

### Variables and Data Structures

#### Fragment Structure
```c
typedef struct s_fragment {
    char *text;              // Fragment text content
    t_quote_type quote_type; // Type of quote (NONE, SINGLE, DOUBLE, MIXED)
    struct s_fragment *next; // Next fragment in the list
} t_fragment;
```

#### Token Structure
```c
typedef struct s_token {
    t_token_type type;      // Token type (WORD, PIPE, REDIRECT, etc.)
    t_fragment *fragments;  // Linked list of fragments that make up the token
    struct s_token *next;   // Next token in the list
} t_token;
```

### Step-by-Step Processing

For the input `echo $'USER'`:

1. **Fragment Creation**:
   - First fragment: `text="echo"`, `quote_type=QUOTE_NONE`
   - Second fragment: `text="$'USER'"`, `quote_type=QUOTE_NONE` (special handling later)

2. **Token Creation**:
   - First token: `type=TOKEN_WORD`, fragments -> "echo"
   - Second token: `type=TOKEN_WORD`, fragments -> "$'USER'"

3. **Output**:
   ```
   Token List:
   - TOKEN_WORD: "echo"
   - TOKEN_WORD: "$'USER'"
   ```

### Debug Representation

If we were to print the token list using `print_token_list()`, we would see:
```
Token: WORD, Value: "echo"
Token: WORD, Value: "$'USER'"
```

### Source Code Example

```c
t_token *tokenize_input(const char *input)
{
    t_fragment *frags = parse_fragments(input);
    t_token *tokens = NULL;
    t_token *tok;
    int type;
    int consumed;
    
    while (frags)
    {
        // Skip empty fragments
        if (strlen(frags->text) == 0)
        {
            frags = frags->next;
            continue;
        }
        
        // Detect token type
        if (frags->quote_type == QUOTE_NONE)
            type = detect_token_type(frags->text);
        else
            type = TOKEN_WORD;
        
        // Create token with the current fragment
        tok = create_token(frags, type);
        
        // Add to token list
        append_token(&tokens, tok);
        
        // Move to next fragment
        frags = NULL; // Simplified for example (would actually move to next fragment)
    }
    
    return tokens;
}
```

## Detailed ANSI-C Quote Handling

For the fragment `$'USER'`:
1. During parsing, it's initially treated as a single fragment
2. Later, during variable expansion, the `$'` is recognized as the start of ANSI-C quoting
3. The content between the quotes is processed according to ANSI-C rules (escape sequence interpretation)
4. For simple cases like 'USER' with no escape sequences, it's treated as a literal string
