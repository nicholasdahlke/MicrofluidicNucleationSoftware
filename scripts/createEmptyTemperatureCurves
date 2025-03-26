#!/bin/bash

# Check if at least one folder is provided
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <folder1> [folder2 ...]"
    exit 1
fi

# Iterate through all given folders
for folder in "$@"; do
    if [ -d "$folder" ]; then
        # Find .mp4 files and create .tc files with replaced extension
        find "$folder" -type f -name "*.mp4" | while read -r file; do
            touch "${file%.mp4}.tc"
        done
    else
        echo "Warning: '$folder' is not a valid directory. Skipping."
    fi
done

echo "Done."
