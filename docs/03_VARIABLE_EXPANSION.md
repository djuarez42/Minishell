# Variable Expansion Component

## Overview

The variable expansion component is responsible for processing environment variables, special parameters, and quote-specific expansion rules. This is particularly important for handling different types of quotes and their effects on expansion.

## Example: Processing `echo $'USER'`

### Input Command Argument

```
$'USER'
```

### Expansion Flow

1. The argument `$'USER'` is passed to `expand_variables()`
2. The initial `$'` is recognized as an ANSI-C quote marker
3. The content between the quotes is processed according to ANSI-C rules
4. Since there are no escape sequences, 'USER' is returned as a literal string

### Key Functions

| Function | Purpose | Example Call |
|----------|---------|-------------|
| `expand_variables()` | Main entry point for variable expansion | `expanded = expand_variables("$'USER'", envp, state, QUOTE_NONE)` |
| `handle_ansi_c_quotes()` | Processes ANSI-C quotes ($'...') | `result = handle_ansi_c_quotes("$'USER'", &i)` |
| `expand_exit_status()` | Expands $? to last exit status | `status_str = expand_exit_status(state)` |
| `env_get_value()` | Gets the value of an environment variable | `value = env_get_value(envp, "USER")` |
| `preprocess_dollar_quotes()` | Pre-processes $ quote combinations | `processed = preprocess_dollar_quotes(arg, &i, envp, state)` |

### Variables and Data Structures

```c
typedef struct s_exec_state {
    int last_status;         // Last command exit status (for $?)
} t_exec_state;
```

### Step-by-Step Processing

For the argument `$'USER'`:

1. **Initial Processing**:
   - `expand_variables()` scans character by character
   - It encounters '$' followed by a single quote

2. **ANSI-C Quote Handling**:
   - `handle_ansi_c_quotes()` is called to process the ANSI-C quoted string
   - The function skips the "$'" prefix
   - It processes characters until the closing single quote
   - For 'USER', there are no escape sequences to process
   - The literal string "USER" is returned

3. **Output**:
   - The expanded string is "USER"

### Source Code Example

```c
char *expand_variables(const char *input, char **envp, t_exec_state *state, t_quote_type quote)
{
    char *result = NULL;
    int i = 0;
    
    while (input[i])
    {
        // Handle $ for variable expansion
        if (input[i] == '$' && quote != QUOTE_SINGLE)
        {
            // ANSI-C quote ($'...')
            if (input[i+1] == '\'')
            {
                char *ansi_c_str = handle_ansi_c_quotes(input, &i);
                result = ft_strjoin_free(result, ansi_c_str);
                free(ansi_c_str);
                continue;
            }
            
            // Other variable expansions...
            // $?, $NAME, etc.
        }
        else
        {
            // Add regular character
            char tmp[2] = {input[i], '\0'};
            result = ft_strjoin_free(result, tmp);
            i++;
        }
    }
    
    return result;
}

char *handle_ansi_c_quotes(const char *input, int *i)
{
    char *result = ft_strdup("");
    
    // Skip $' prefix
    (*i) += 2;
    
    // Process until closing quote
    while (input[*i] && input[*i] != '\'')
    {
        if (input[*i] == '\\')
        {
            (*i)++;  // Skip backslash
            
            // Handle escape sequences
            // (For this example, there are none)
        }
        else
        {
            // Add regular character
            char tmp[2] = {input[*i], '\0'};
            result = ft_strjoin_free(result, tmp);
            (*i)++;
        }
    }
    
    // Skip closing quote
    if (input[*i] == '\'')
        (*i)++;
        
    return result;
}
```

### ANSI-C Quote Rules

ANSI-C quoting (`$'...'`) allows for special character escapes:

| Escape Sequence | Meaning | Example |
|-----------------|---------|---------|
| `\a` | Alert (bell) | `$'\a'` |
| `\b` | Backspace | `$'\b'` |
| `\e` | Escape character | `$'\e'` |
| `\f` | Form feed | `$'\f'` |
| `\n` | Newline | `$'\n'` |
| `\r` | Carriage return | `$'\r'` |
| `\t` | Horizontal tab | `$'\t'` |
| `\v` | Vertical tab | `$'\v'` |
| `\\` | Backslash | `$'\\'` |
| `\'` | Single quote | `$'\''` |
| `\"` | Double quote | `$'\"'` |
| `\nnn` | Octal value | `$'\101'` (A) |
| `\xHH` | Hexadecimal value | `$'\x41'` (A) |

### Detailed ANSI-C Processing Example

For a more complex ANSI-C quote example like `$'USER\nHOME'`:

1. Start with empty result
2. Process 'U' → result = "U"
3. Process 'S' → result = "US"
4. Process 'E' → result = "USE"
5. Process 'R' → result = "USER"
6. Process '\n' → result = "USER\n" (actual newline)
7. Process 'H' → result = "USER\nH"
8. ... and so on

### Quote Type Effects on Expansion

Different quote types affect variable expansion:

| Quote Type | Variable Expansion | Example Input | Example Output |
|------------|-------------------|---------------|----------------|
| No quotes | Yes | `$USER` | `ekakhmad` |
| Single quotes | No | `'$USER'` | `$USER` |
| Double quotes | Yes | `"$USER"` | `ekakhmad` |
| ANSI-C quotes | No, but escape processing | `$'$USER'` | `$USER` |
| ANSI-C with escapes | Escape processing only | `$'\\n$USER'` | `\n$USER` |
