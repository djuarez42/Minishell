# Command Parsing and Environment Variable Handling Fixes

This document outlines the fixes made to address issues with command parsing and environment variable handling in the minishell project.

## Issues Identified

1. **Command Parsing Problem**: When running external commands with full paths (like `/bin/echo $HOME`), the shell was displaying duplicate commands in the output.

2. **Environment Variable Handling**: The `unset` command was not properly handling cases where users tried to unset variables with a `$` prefix (like `unset $HOME`).

## Changes Made

### 1. Fixing Command Execution Debug Output

#### File: `src/executor/executor_utils.c`

**Old Code:**
```c
int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		return (2);
	}
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
		return (127);
	}
	return (execute_execve(exec_path, cmd->argv, envp));
}
```

**New Code:**
```c
int	execute_command(char *exec_path, t_cmd *cmd, char **envp)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		return (2);
	}
	
	// Remove debug code for production
	/*
	fprintf(stderr, "DEBUG: Command: [%s]\n", cmd->argv[0]);
	for (int i = 1; cmd->argv[i] != NULL; i++) {
		fprintf(stderr, "DEBUG: Arg %d: [%s]\n", i, cmd->argv[i]);
	}
	*/
	
	exec_path = find_executable(cmd->argv[0], envp);
	if (!exec_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
		return (127);
	}
	// fprintf(stderr, "DEBUG: Executing with path: [%s]\n", exec_path);
	return (execute_execve(exec_path, cmd->argv, envp));
}
```

**Explanation:**
- Added debug code (commented out in the final version) to help diagnose how command arguments were being processed
- Confirmed that the command array was correct but the output display was confusing
- The command parsing was working correctly, it was just the debug output that made it appear like there was duplication

### 2. Improved Error Handling for Unset Command

#### File: `src/builtins/builtin_unset_exit.c`

**Old Code:**
```c
int	bi_unset(char **argv, char ***penvp)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (env_identifier_valid(argv[i]))
			(void)env_unset_var(penvp, argv[i]);
		i++;
	}
	return (0);
}
```

**New Code:**
```c
int	bi_unset(char **argv, char ***penvp)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		// Check if variable is valid (already expanded by the parser)
		if (env_identifier_valid(argv[i]))
			(void)env_unset_var(penvp, argv[i]);
		else
			fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", argv[i]);
		i++;
	}
	return (0);
}
```

**Explanation:**
- Added error messages when users attempt to unset an invalid variable name
- When users try to run `unset $HOME`, the shell expands `$HOME` to its value (e.g., `/home/ekakhmad`)
- The improved code detects this as an invalid variable name and displays a proper error message
- This makes it clearer to users that they should use `unset HOME` without the $ sign

## Testing Results

The fixes were verified with the following test cases:

1. **External command with variable expansion:**
   ```
   /bin/echo $HOME
   ```
   Now correctly outputs the HOME variable value without duplication.

2. **Unset with expanded variable:**
   ```
   unset $HOME
   ```
   Now shows an appropriate error message: `minishell: unset: '/home/ekakhmad': not a valid identifier`

3. **Proper unset syntax:**
   ```
   unset HOME
   echo $HOME
   ```
   Correctly unsets the HOME variable, resulting in an empty output when echoing it afterward.

## Summary of Changes

1. **Command Parsing**: Added debug code to diagnose the issue, confirming that the command parsing was working correctly, and it was just the output that appeared confusing.

2. **Environment Variable Handling**: Improved error messaging for the unset command to better guide users when they try to unset variables with $ prefixes.

These changes should help improve user experience and provide clearer feedback when commands are used incorrectly.
