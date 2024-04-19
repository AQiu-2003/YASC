#!/bin/bash

# keyboard input directory name
echo -n "Enter the directory name: "
read dir

# read all files in ./Sample/ directory
for file in ./Sample/$dir/*; do
    # check if the file is a regular file
    if [ -f "$file" ]; then
        # check if the file is a text file
        if [ -n "$(file "$file" | grep text)" ]; then
            # print the file name
            echo -e "\nProcessing file: $file"
            # run the parser on the file
            ./build/YASC --no-tree "$file"
        fi
    fi
done