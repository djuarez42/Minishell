export A=1 B=two
env | grep -E '^(A|B)='
unset A
env | grep -E '^(A|B)=' | sort