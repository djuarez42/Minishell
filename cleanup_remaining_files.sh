#!/bin/bash

# Create backup directory for unused files
BACKUP_DIR="./unused_files_backup"
mkdir -p "$BACKUP_DIR"

# List of files not included in Makefile but still present in workspace
declare -a UNUSED_FILES=(
    "src/lexer/lexer_utils.c"
    "src/lexer/lexer_utils_3.c"
    "src/lexer/lexer_utils_4.c"
    "src/lexer/dollar_quotes_special.c"
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
