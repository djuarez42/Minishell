int bi_env(char **argv, t_env *env)
{
	// ...existing code...
	// If you don't support options/args for env, ignore argv[1+] or error out.
	// Printing must be KEY=VALUE to STDOUT, one per line.
	t_env *cur = env;
	while (cur) {
		if (cur->value != NULL) {
			dprintf(STDOUT_FILENO, "%s=%s\n", cur->key, cur->value);
		}
		cur = cur->next;
	}
	return 0;
}
