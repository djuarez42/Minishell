#!/bin/bash

# Simple test for the $"string" handling

# Test in bash
echo "=== Testing in bash ==="
echo "echo \$\"HOME\" output:"
bash -c 'echo $"HOME"'
echo

# Test in our minishell
echo "=== Testing in our minishell ==="
echo "echo \$\"HOME\" output:"
./minishell < <(echo 'echo $"HOME"')
echo

echo "=== Done ==="
