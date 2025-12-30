#!/bin/bash

# Directory to search
TARGET_DIR="$1"

# Output files
OUTPUT_FILES_LIST="files.txt"
OUTPUT_QRC_FILE="$1.qrc"

mv $1/.git $1/_git
# Find all files in the target directory and save them to files.txt, excluding the directory prefix
find "$TARGET_DIR" -type f | sed "s|^$TARGET_DIR/||" > "$OUTPUT_FILES_LIST"

# Create the repo.qrc file
echo '<!DOCTYPE RCC>' > "$OUTPUT_QRC_FILE"
echo '<RCC>' >> "$OUTPUT_QRC_FILE"
echo '    <qresource>' >> "$OUTPUT_QRC_FILE"

# Read each file from files.txt and append to the qrc file with the dir_name prefix
while IFS= read -r file; do
    if [[ $file == _git/* ]]; then
        # Remove the "_git/" prefix and replace it with ".git/"
        alias_path=".git/${file#_git/}"
        echo "        <file alias=\"$TARGET_DIR/$alias_path\">$TARGET_DIR/$file</file>" >> "$OUTPUT_QRC_FILE"
    else
        echo "        <file>$TARGET_DIR/$file</file>" >> "$OUTPUT_QRC_FILE"
    fi
done < "$OUTPUT_FILES_LIST"

# Add files.txt itself to the qrc file
echo "        <file>$OUTPUT_FILES_LIST</file>" >> "$OUTPUT_QRC_FILE"

# Close the qresource and RCC tags
echo '    </qresource>' >> "$OUTPUT_QRC_FILE"
echo '</RCC>' >> "$OUTPUT_QRC_FILE"

# Confirmation message
echo "All files in '$TARGET_DIR' have been saved to '$OUTPUT_FILES_LIST'."
echo "Qt resource file '$OUTPUT_QRC_FILE' has been created, including '$OUTPUT_FILES_LIST'."
