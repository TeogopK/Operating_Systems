#!/bin/bash

# Define the directory path
directory="."

# Change directory to the specified path
cd "$directory" || exit

# Create or overwrite the all.txt file
> all.txt

# Loop through files starting with '03' in the directory
for file in 03*; do
    # Print the file name to all.txt
    echo "Task: $file" >> all.txt
    # Concatenate the file contents and append to all.txt
    cat "$file" >> all.txt
    # Add a newline for separation
    echo -e "\n" >> all.txt
done
