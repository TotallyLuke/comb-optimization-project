#!/bin/bash

# Directory containing the input files
input_directory="test_instances"

# Function to run the program and measure time
run_test() {
    local input_file=$1
    echo "Running with $input_file:"
    /usr/bin/time -v ./assignment2 -f "$input_file"
    echo ""
}

# Loop through all files in the input directory
for input_file in "$input_directory"/*; do
    if [[ -f "$input_file" ]]; then  # Check if it's a file
        run_test "$input_file"
    fi
done

