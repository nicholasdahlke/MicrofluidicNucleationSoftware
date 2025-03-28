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

    # Create folder for the old videos
    folder=$(dirname -- "$input_file")
    mkdir "$folder/old_videos" -v
    mv "$input_file" "$folder/old_videos"

    filename=$(basename -- "$input_file")
    file="$folder/old_videos/$filename"

    # Apply horizontal mirroring using ffmpeg
    ffmpeg -i "$file" -vf eq=gamma=0.4 -c:a copy "$input_file"

    echo "Gamma corrected video saved as: $input_file"
done
