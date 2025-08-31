# Minishell Lexer and Tokenizer Architecture

This document provides a detailed analysis of the current lexer/tokenizer architecture and proposes an improved architecture for better handling of quotes, backslashes, and variable expansion.

## Current Architecture

The current implementation follows a multi-stage process for tokenization and command parsing.

### Current Flow Diagram

```mermaid
flowchart TD
    A[User Input] --> B[tokenize_input]
    B --> C[clean_input_quotes]
    C --> D[reconstruct_words]
    D --> E[build_token_list]
    E --> F[Token List]
    F --> G[parser_tokens]
    G --> H[Command Structure]
    H --> I[expand_cmd_inplace]
    I --> J[Execute Commands]
    
    subgraph "Tokenization Phase"
        B
        C
        D
        E
    end
    
    subgraph "Parsing Phase"
        G
    end
    
    subgraph "Expansion Phase"
        I
    end
    
    subgraph "Execution Phase"
        J
    end
```

### Current Quote Handling Process

```mermaid
flowchart LR
    A[Input with Quotes] --> B[Track Quote State]
    B --> C{Quote Type?}
    C -->|None| D[Process as normal]
    C -->|Single| E[Preserve literal content]
    C -->|Double| F[Preserve with variable expansion]
    
    D --> G[build_token_list]
    E --> G
    F --> G
    
    G --> H[Command Structure]
    H --> I[Expansion Phase]
    I -->|Variables in Double Quotes| J[Expand Variables]
    I -->|Variables in Single Quotes| K[Keep Literal]
```

### Current Expansion Process

```mermaid
flowchart TD
    A[Command Structure] --> B[expand_cmd_inplace]
    B --> C[Iterate through Arguments]
    C --> D{Quote Type?}
    D -->|None or Double| E[expand_variables]
    D -->|Single| F[Keep Literal]
    E --> G{Character Type?}
    G -->|Regular| H[Add to Result]
    G -->|$| I[handle_dollar]
    G -->|Backslash + $| J[Add Literal $]
    I --> K[Expand Variable]
    
    K --> H
    J --> H
    F --> L[Final Result]
    H --> L
```

## Proposed Improved Architecture

The proposed architecture enhances the handling of quotes, backslashes, and variables with a more unified approach.

### Proposed Flow Diagram

```mermaid
flowchart TD
    A[User Input] --> B[Character-by-Character Lexer]
    B --> C[Quote-Aware Tokenizer]
    C --> D[Raw Tokens]
    D --> E[Token Analyzer]
    E --> F[Processed Tokens]
    F --> G[Variable Expander]
    G --> H[Expanded Tokens]
    H --> I[Parser]
    I --> J[Command Structure]
    J --> K[Execute Commands]
    
    subgraph "Enhanced Lexer Phase"
        B
        C
        D
    end
    
    subgraph "Token Processing Phase"
        E
        F
    end
    
    subgraph "Variable Expansion Phase"
        G
        H
    end
    
    subgraph "Parsing Phase"
        I
        J
    end
    
    subgraph "Execution Phase"
        K
    end
```

### Proposed Quote Handling Process

```mermaid
flowchart TD
    A[Input Character] --> B{In Quote?}
    B -->|No| C{Is Quote Character?}
    B -->|Yes| D{Matches Current Quote?}
    
    C -->|Yes| E[Start Quote Context]
    C -->|No| F{Is Backslash?}
    
    D -->|Yes| G[End Quote Context]
    D -->|No| H[Add to Current Token]
    
    F -->|Yes| I[Set Escaped Flag]
    F -->|No| J[Process Regular Character]
    
    E --> K[Update Quote Context]
    G --> K
    I --> K
    
    H --> L[Build Token]
    J --> L
    
    K --> M[Next Character]
    L --> M
```

### Proposed Expansion Process

```mermaid
flowchart TD
    A[Token with Context] --> B{Has Variables?}
    B -->|No| C[Keep Original Text]
    B -->|Yes| D{Quote Type?}
    
    D -->|None| E[Full Expansion]
    D -->|Single| F[No Expansion]
    D -->|Double| G[Variable-Only Expansion]
    
    E --> H[Scan for $ Characters]
    G --> H
    
    H --> I{Is $ Escaped?}
    I -->|Yes| J[Add Literal $]
    I -->|No| K[Expand Variable]
    
    K --> L[Replace in Result]
    J --> L
    F --> M[Final Result]
    C --> M
    L --> M
```

## Unified Context Structure

The proposed architecture introduces a unified context structure to track the state during lexing and tokenization:

```c
typedef struct s_quote_context {
    t_quote_type current_quote;    // Current active quote (NONE, SINGLE, DOUBLE)
    bool         escaped;          // Is next character escaped by backslash
    bool         expand_variables; // Should variables be expanded in this context
} t_quote_context;
```

## Enhanced Token Structure

The tokens are enhanced with additional information to facilitate proper handling:

```c
typedef struct s_enhanced_token {
    char         *raw_text;        // Original unprocessed text
    char         *processed_text;  // Text after quote removal
    t_quote_type quote_type;       // Type of quote surrounding this token
    bool         has_variables;    // Does token contain expandable variables
    t_token_type type;             // Type of token (WORD, OPERATOR, etc.)
    struct s_enhanced_token *next; // Next token in the list
} t_enhanced_token;
```

## Rules for Special Character Handling

### Backslash Rules

1. **Outside quotes**:
   - Backslash escapes any following character
   - `\$` → literal `$`
   - `\\` → literal `\`
   - `\"` → literal `"`
   - `\'` → literal `'`

2. **Inside double quotes**:
   - Backslash only escapes `$`, `"`, `\`, and newline
   - `\$` → literal `$`
   - `\\` → literal `\`
   - `\"` → literal `"`
   - `\'` → `\'` (backslash + quote)

3. **Inside single quotes**:
   - Backslash has no special meaning
   - `\$` → `\$` (backslash + dollar)
   - `\\` → `\\` (two backslashes)
   - `\"` → `\"` (backslash + quote)

### Variable Expansion Rules

1. **Outside quotes**:
   - Variables are expanded
   - `$USER` → value of USER

2. **Inside double quotes**:
   - Variables are expanded
   - `"$USER"` → value of USER

3. **Inside single quotes**:
   - No variable expansion
   - `'$USER'` → literal `$USER`

## Comparison: Current vs. Proposed

| Aspect | Current Implementation | Proposed Implementation |
|--------|------------------------|-------------------------|
| Quote Handling | Spread across multiple phases | Unified context structure |
| Backslash Processing | Mixed in with variable expansion | Dedicated character processor |
| Variable Expansion | During execution phase | Separate dedicated phase |
| Token Information | Basic token types | Enhanced with quote context and expansion flags |
| Error Handling | Various locations | Centralized during tokenization |
| Complex Cases | Ad-hoc handling | Systematic rules-based approach |

## Implementation Strategy

1. Implement the `t_quote_context` structure
2. Create the character-by-character processor
3. Enhance the token structure
4. Update the variable expansion logic
5. Integrate the improved components into the existing codebase

By following this architecture, the minishell will have a more robust and maintainable approach to handling complex cases with quotes, backslashes, and variable expansions.
