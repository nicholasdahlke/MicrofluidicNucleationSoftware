#!/bin/bash

# Check if at least one file is provided
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <video1> <video2> ..."
    exit 1
fi

# Loop through each file provided as an argument
for input_file in "$@"; do
    # Check if file exists
    if [ ! -f "$input_file" ]; then
        echo "File not found: $input_file"
        continue
    fi

    # Extract filename without extension and extension
    filename="${input_file%.*}"
    extension="${input_file##*.}"
    output_file="${filename}_g.${extension}"

    # Apply horizontal mirroring using ffmpeg
    ffmpeg -i "$input_file" -vf eq=gamma=0.4 -c:a copy "$output_file"

    echo "Mirrored video saved as: $output_file"
done
