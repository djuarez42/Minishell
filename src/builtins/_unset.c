#include "builtins.h"
#include "env.h"
#include <stdio.h>

// ...existing code...

static void print_invalid(const char *arg) {
	fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", arg);
}

int bi_unset(struct s_shell *sh, char **argv) {
	int status = 0;

	// argv[0] == "unset"
	for (size_t i = 1; argv[i]; ++i) {
		const char *key = argv[i];
		if (!is_valid_identifier(key)) {
			print_invalid(key);
			status = 1;
			continue;
		}
		(void)env_unset(sh, key);
	}
	return status;
}
