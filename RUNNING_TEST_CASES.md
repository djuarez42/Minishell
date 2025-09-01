# Running Specific Test Cases in Minishell

## How to Run Specific Test Cases

To run specific test cases in Minishell, you can use one of these methods:

### 1. Using the `-c` Option

Run a specific command directly:

```bash
./minishell -c "your command here"
```

For example:
```bash
./minishell -c "echo $\"HOME\"$USER"
```

### 2. Using a Test Script

Create and run a test script like `test_builtin_cases.sh`:

```bash
#!/bin/bash

cd /home/ekakhmad/Minishell

echo "Running specific test cases:"

# Test case 1
echo -e "\n1. echo \"'\"'\$USER'\"'\""
echo -n "Bash: "
bash -c 'echo "'"'"$USER"'"'"'
echo -n "Minishell: "
./minishell -c "echo \"'\"'\$USER'\"'\""

# Add more test cases as needed
```

Then run it:
```bash
chmod +x test_builtin_cases.sh
./test_builtin_cases.sh
```

### 3. Using Input Redirection

```bash
printf "your command\n" | ./minishell
```

For example:
```bash
printf "echo $\"HOME\"$USER\n" | ./minishell
```

## Current Status of Test Cases

From the `1_builtins.sh` test file, here's the current status:

1. `echo "'"'$USER'"'"` - **Working correctly**
2. `echo '"'"$USER"'"'` - **Has issues** with quote handling
3. `echo $"HOME"$USER` - **Has issues** (expanding `$HOME` instead of treating it as a literal string)
4. `echo $"HOM"E$USER` - **Has issues** (same as above)
5. `echo "exit_code ->$? user ->$USER home -> $HOME"` - **Working correctly**

## Known Issues

1. **$"string" Syntax**: In bash, this is used for locale translation but should be treated as a literal string without variable expansion. In Minishell, it's currently expanding variables instead.

2. **Complex Quote Combinations**: Some complex combinations of quotes are not handled correctly.

## How to Fix the Issues

To fix these issues, you would need to:

1. Modify the `expand_variables.c` file to handle the `$"string"` syntax specially
2. Improve quote handling in complex nested cases

A potential fix for the `$"string"` syntax would involve detecting this pattern and treating it differently from normal variable expansion.

## Running Tests with the 42_minishell_tester

If you have the 42_minishell_tester installed:

```bash
cd /home/ekakhmad/Minishell/42_minishell_tester
./tester.sh m b  # Test builtins
```
