# Minishell Test Case Extractor

This tool helps you identify and understand failing tests in your Minishell project by extracting test cases from the 42_minishell_tester output.

## Features

- Extracts failing test cases from tester output files
- Shows the specific test command that failed with context
- Compares bash output with minishell output for each failing test
- Creates organized text and markdown reports
- Filters by test type (stdout, stderr, exit code)
- Helps track and fix test failures over time
- Side-by-side comparison of bash and minishell output

## Usage

```bash
# Basic usage (extracts failing stdout tests by default)
./extract_test_cases.sh pa2.txt

# Extract failing stderr tests
./extract_test_cases.sh pa2.txt STD_ERR

# Extract failing exit code tests
./extract_test_cases.sh pa2.txt EXIT_CODE
```

## Output Files

The script creates two files in the `test_results` directory:

1. **Text report**: Contains the detailed test failures with context and output comparison
   - Format: `test_results/STD_OUT_failures_YYYYMMDD_HHMMSS.txt`
   - Includes bash vs minishell output comparison for each test

2. **Markdown summary**: Provides a formatted summary of test failures
   - Format: `test_results/STD_OUT_summary_YYYYMMDD_HHMMSS.md`
   - Includes tables, categories, and tips for fixing issues

## Output Comparison

For each failing test, the script:

1. Runs the command in bash and captures stdout, stderr, and exit code
2. Runs the same command in your minishell and captures the same data
3. Displays side-by-side comparison to help identify differences

This makes it much easier to see exactly where your minishell implementation differs from the expected bash behavior.

## Common Test Categories

### Quote Handling
Tests related to handling single and double quotes, nested quotes, and escaped quotes.

### Variable Expansion
Tests for proper $VAR expansion in different contexts (quotes, command substitution).

### Command Execution
Tests for proper command parsing and execution.

### Redirections
Tests for input/output redirections (<, >, >>, <<).

### Pipes
Tests for proper pipe handling (|).

## Tips for Fixing Tests

1. Run the extract script to identify failing tests
2. Compare your Minishell output with Bash output
3. Check the test file to understand expected behavior
4. Debug your code with the identified test cases
5. Fix one category of issues at a time
6. Re-run tests to verify your fixes

## Examples

### Finding Quote Handling Issues

```bash
# Generate test output first
bash ./tester.sh m pa > pa2.txt

# Extract failing tests
./extract_test_cases.sh pa2.txt
```

### Tracking Progress

Compare the output of the script over time to see your progress:

```bash
# Before fixes
./extract_test_cases.sh before_fixes.txt > progress_report.txt

# After fixes
./extract_test_cases.sh after_fixes.txt >> progress_report.txt
```

## Creating Your Own Test Cases

You can add test cases to the tester by adding commands to the appropriate test files in the `cmds/mand/` directory.
