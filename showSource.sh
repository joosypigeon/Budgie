#!/bin/bash

# Start from the current directory
DIR="."

# Find relevant files while pruning unwanted directories
FILES=$(find "$DIR" \
    -type d \( -name build -o -name unity -o -name .git -o -name .vscode \) -prune -o \
    -type f \( -name "*.c" -o -name "*.h" -o -name "*.txt" \) -print | sort)

# Output file contents to output.txt
for FILE in $FILES; do
    echo "===== $FILE ====="
    cat "$FILE"
    echo
done > output.txt
