#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file_to_compress>"
    exit 1
fi

INPUT_FILE="$1"
COMPRESSED_FILE="${INPUT_FILE}.huff"
DECOMPRESSED_FILE="${INPUT_FILE}.orig"

make run args="-c $INPUT_FILE"

make run args="-dc $COMPRESSED_FILE"

echo "Comparing '$INPUT_FILE' and '$DECOMPRESSED_FILE'..."
if cmp -s "$INPUT_FILE" "$DECOMPRESSED_FILE"; then
    echo "Test passed: '$INPUT_FILE' and '$DECOMPRESSED_FILE' are identical."
else
    echo "Test failed: '$INPUT_FILE' and '$DECOMPRESSED_FILE' differ."
    exit 1
fi
