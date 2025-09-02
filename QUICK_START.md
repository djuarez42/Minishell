# Quick Start Guide for Minishell Testing

## Overview
This guide helps you systematically test and fix the failing minishell cases using the created diagnostic tools.

## File Structure
```
/home/ekakhmad/Minishell/
├── TESTING_PLAN.md           # Complete analysis plan
├── diagnostic_part1.sh       # Core functionality tests
├── diagnostic_part2.sh       # Advanced feature tests
└── tests0209.txt            # Original failing test cases
```

## Quick Test Execution

### Step 1: Test Core Issues (Part 1)
```bash
cd /home/ekakhmad/Minishell
./diagnostic_part1.sh > part1_results.txt 2>&1
```

**Focus Areas:**
- ❌ Tilde expansion (`cd ~`, `echo ~`)
- ❌ Quote/variable mixing (`echo '"'$USER'"'`)
- ❌ Dollar quotes (`/bin/echo $"HOME"`)

### Step 2: Test Advanced Issues (Part 2)
```bash
./diagnostic_part2.sh > part2_results.txt 2>&1
```

**Focus Areas:**
- ❌ Escape sequences (`/bin/echo \$USER`)
- ❌ Complex quote combinations
- ❌ Variable assignment edge cases

## Priority Fix Order

### 🔴 Critical (Fix First)
1. **Tilde Expansion** - Affects `cd ~` and path resolution
2. **Basic Quote/Variable Mixing** - Core echo functionality
3. **Dollar Quote Syntax** - `$"string"` handling

### 🟡 Important (Fix Second)  
4. **Escape Sequences** - Backslash handling
5. **Complex Variable Expansion** - Edge cases
6. **Export/Whitespace Handling** - Environment variables

### 🟢 Enhancement (Fix Last)
7. **Complex Quote Nesting** - Advanced syntax
8. **Special Character Handling** - Edge cases

## Expected Locations for Fixes

### Tilde Expansion
- `src/executor/expand_variables.c` - Add `~` detection and expansion
- `src/builtins/builtin_cd.c` - Ensure cd handles expanded paths

### Quote/Variable Issues  
- `src/lexer/tokenizer.c` - Quote state management
- `src/executor/expand_mixed_quotes.c` - Quote processing order

### Dollar Quotes
- `src/executor/dollar_quotes_fix.c` - `$"string"` processing
- `src/executor/preprocess_dollar_quotes.c` - Preprocessing logic

### Escape Sequences
- `src/lexer/tokenizer.c` - Backslash parsing
- `src/executor/expand_variables.c` - Escape handling during expansion

## Testing Workflow

1. **Run diagnostic script**
2. **Identify specific failures**
3. **Implement targeted fix**
4. **Test fix with diagnostic script**
5. **Run full tester**: `cd 42_minishell_tester && bash tester.sh m b`
6. **Repeat for next priority issue**

## Success Metrics

- **Part 1 Complete**: 80%+ basic functionality working
- **Part 2 Complete**: 95%+ advanced features working
- **Overall Goal**: Pass majority of 42_minishell_tester tests

## Quick Commands

```bash
# Test specific functionality
./diagnostic_part1.sh | grep -A 5 -B 2 "Tilde"

# Compare specific case
echo "echo ~" | ./minishell
bash -c "echo ~"

# Run full tester
cd 42_minishell_tester && timeout 30 bash tester.sh m b | head -50
```

Start with `./diagnostic_part1.sh` and focus on the highest priority issues first!
