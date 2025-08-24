# Normalize by grepping a known var we set
export FOO=bar
env | grep '^FOO='