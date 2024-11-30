#!/bin/bash
# Directories
SRC_DIR="src"
BIN_DIR="bin"

# Compile each .cpp file in the src directory
for file in "$SRC_DIR"/*.cpp; do
    # Get the base name of the file (without path and extension)
    base_name=$(basename "$file" .cpp)
    
    # Compile the file and output to the bin directory
    g++ -o "$BIN_DIR/$base_name" "$file"
    ## Uncomment below if you need .exe files
    #g++ -o "$BIN_DIR/$base_name".exe "$file" 
    
    # Check if compilation succeeded
    if [ $? -eq 0 ]; then
        echo "Compiled $file -> $BIN_DIR/$base_name"
    else
        echo "Failed to compile $file"
    fi
done

echo "Finished Compiling!"
