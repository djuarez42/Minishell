#!/bin/bash

# Create backup directory for unused files
BACKUP_DIR="./unused_files_backup"
mkdir -p "$BACKUP_DIR"

# List of files not included in Makefile
declare -a UNUSED_FILES=(
    "src/builtins/_env.c"
    "src/builtins/_export.c"
    "src/builtins/fix_export_display.c"
    "src/builtins/_unset.c"
    "src/debug_functions.c"
    "src/debug_tokens.c"
    "src/env/env.c"
    "src/exec/exec_command.c"
    "src/executor/debug_fd_manager.c"
    "src/executor/dollar_string_fix_v2.c"
    "src/executor/expand_variables_patch.c"
    "src/executor/expand_variables_utils3.c"
    "src/executor/fd_manager.c"
    "src/executor/heredoc_utils.c"
    "src/executor/locale_string_utils.c"
    "src/executor/pipeline.c"
    "src/executor/redirection.c"
    "src/lexer/_dollar_quotes_special.c"
    "src/lexer/__lexer_utils_3.c"
    "src/lexer/___lexer_utils_4.c"
    "src/lexer/__lexer_utils.c"
    "src/lexer/token_error.c"
    "src/lexer/tokenizer_utils_2.c"
    "src/lexer/tokenizer_utils_3.c"
    "src/lexer/tokenizer_utils.c"
    "src/lexer/token_lexer_utils.c"
    "src/lexer/token_word_validation.c"
    "src/parser/parser_errors.c"
    "src/parser/parser_validate.c"
)

# Function to move a file with its directory structure
move_file() {
    local file=$1
    local dest_dir="$BACKUP_DIR/$(dirname "$file")"
    
    # Create destination directory structure
    mkdir -p "$dest_dir"
    
    # Check if file exists before moving
    if [ -f "$file" ]; then
        echo "Moving $file to $dest_dir/"
        mv "$file" "$dest_dir/"
    else
        echo "Warning: $file does not exist, skipping"
    fi
}

# Move each file
for file in "${UNUSED_FILES[@]}"; do
    move_file "$file"
done

echo "Cleanup complete. Unused files have been moved to $BACKUP_DIR"
echo "To restore files, you can use: 'cp -r $BACKUP_DIR/* ./'"
