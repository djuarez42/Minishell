#!/bin/bash

# Create a test file that isolates the $"string" syntax
cat > locale_specific_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/minishell.h"

// Mock function to see what handle_dollar receives
char *mock_handle_dollar(const char *input, int *i) {
    printf("Input to handle_dollar: '%s'\n", input + *i);
    
    // Check if this is a locale string
    if (input[*i] == '$' && input[*i + 1] == '"') {
        printf("Detected locale string at position %d\n", *i);
        
        // Skip $ and "
        (*i)++;
        (*i)++;
        
        // Find the closing quote
        int start = *i;
        while (input[*i] && input[*i] != '"')
            (*i)++;
        
        // Extract the string between quotes
        char *str = strndup(input + start, *i - start);
        printf("Extracted locale string: '%s'\n", str);
        
        // Skip closing quote if present
        if (input[*i] == '"')
            (*i)++;
        
        return str;
    }
    
    // Just return something for testing
    return strdup("MOCK_RESULT");
}

int main() {
    // Test cases for locale string handling
    const char *test1 = "$\"HOME\"$USER";
    const char *test2 = "$\"HOME is $HOME\"";
    const char *test3 = "echo $\"\\$USER\"";
    
    printf("=== Test 1: %s ===\n", test1);
    int i = 0;
    char *result = mock_handle_dollar(test1, &i);
    printf("Result: %s, Position after handling: %d, Rest: '%s'\n\n", 
           result, i, test1 + i);
    free(result);
    
    printf("=== Test 2: %s ===\n", test2);
    i = 0;
    result = mock_handle_dollar(test2, &i);
    printf("Result: %s, Position after handling: %d, Rest: '%s'\n\n", 
           result, i, test2 + i);
    free(result);
    
    printf("=== Test 3: %s ===\n", test3);
    i = 5; // Skip "echo " to get to $
    result = mock_handle_dollar(test3, &i);
    printf("Result: %s, Position after handling: %d, Rest: '%s'\n\n", 
           result, i, test3 + i);
    free(result);
    
    return 0;
}
EOF

# Compile and run
gcc -o locale_specific_test locale_specific_test.c -Iinclude
./locale_specific_test

# Create a simple direct test for our minishell
cat > direct_locale_test.txt << 'EOF'
echo $"HOME"
echo $"USER"
echo $"HOME"$USER
EOF

echo "=== Testing in minishell ==="
./minishell < direct_locale_test.txt
