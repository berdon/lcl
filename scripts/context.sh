#!/bin/bash

# Search context files and directories for specified tags and copy to clipboard
# Supports file/directory search with tag filtering and exclusion paths
# Tags: context, search, clipboard, tags, bash

# Function to display usage
usage() {
  echo "Usage: $0 [-f file_or_directory]... [-t tag]... [-x exclude_path]..."
  echo "  -f: Specify file or directory to include in search"
  echo "  -t: Specify tag to search for"
  echo "  -x: Specify directory or file to exclude"
  exit 1
}

# Check if pbcopy is available
if ! command -v pbcopy &> /dev/null; then
  echo "Error: pbcopy not found. This script requires pbcopy to copy to clipboard."
  exit 1
fi

# Check if dd is available
if ! command -v dd &> /dev/null; then
  echo "Error: dd not found. This script requires dd for chunking."
  exit 1
fi

# Parse command line arguments
paths=()
tags=()
exclude_paths=()
while getopts "f:t:x:" opt; do
  case $opt in
    f) paths+=("$OPTARG");;
    t) tags+=("$OPTARG");;
    x) exclude_paths+=("$OPTARG");;
    \?) usage;;
  esac
done

# Validate inputs
if [ ${#paths[@]} -eq 0 ] || [ ${#tags[@]} -eq 0 ]; then
  echo "Error: At least one file or directory and one tag must be specified."
  usage
fi

# Function to check if a path should be excluded
check_excluded() {
  local path="$1"
  for exclude_path in "${exclude_paths[@]}"; do
    if [[ "$path" == "$exclude_path" || "$path" == "$exclude_path/"* ]]; then
      return 0
    fi
  done
  return 1
}

# Function to check if a file contains any of the specified tags
check_tags() {
  local file="$1"
  local tag_found=false

  # Read the file and look for Tags line
  while IFS= read -r line; do
    if [[ $line =~ "Tags:" ]]; then
      # Extract tags from the line
      tags_line=$(echo "$line" | sed 's/Tags: //; s/, / /g; s/,//g')
      for tag in "${tags[@]}"; do
        if echo "$tags_line" | grep -qw "$tag"; then
          tag_found=true
          break
        fi
      done
      break
    fi
  done < "$file"
  $tag_found
}

# Create temporary file
tempfile=$(mktemp /tmp/context.XXXXXX)
if [ $? -ne 0 ]; then
  echo "Error: Could not create temporary file."
  exit 1
fi

echo "Using these files for context..." >> "$tempfile"

# Collect all matching file contents into temp file
found=false

# Process all paths (files and directories)
for path in "${paths[@]}"; do
  if [ ! -e "$path" ]; then
    echo "Warning: Path '$path' does not exist, skipping."
    continue
  fi

  # Handle directories
  if [ -d "$path" ]; then
    # Find all files recursively and check for tags
    while IFS= read -r file; do
      # Skip excluded paths
      if check_excluded "$file"; then
        continue
      fi
      if check_tags "$file"; then
        echo "$file" >> "$tempfile"
        echo "\`\`\`" >> "$tempfile"
        cat "$file" >> "$tempfile"
        echo -e "\n\`\`\`" >> "$tempfile"
        echo -e "\n\n" >> "$tempfile"
        found=true
      fi
    done < <(find "$path" -type f)
  # Handle files
  elif [ -f "$path" ]; then
    if check_excluded "$path"; then
      echo "Warning: File '$path' is excluded, skipping."
      continue
    fi
    if check_tags "$path"; then
      echo "$path" >> "$tempfile"
      echo "\`\`\`" >> "$tempfile"
      cat "$path" >> "$tempfile"
      echo -e "\n\`\`\`" >> "$tempfile"
      echo -e "\n\n" >> "$tempfile"
      found=true
    fi
  fi
done

# If no content found, exit
if ! $found; then
  echo "No files found containing the specified tags."
  rm "$tempfile"
  exit 0
fi

printf "=====\n\n\n" >> "$tempfile"

# Get file size (assuming macOS stat)
filesize=$(stat -f%z "$tempfile" 2>/dev/null)
# shellcheck disable=SC2181
if [ $? -ne 0 ]; then
  # Fallback for Linux
  filesize=$(stat -c%s "$tempfile")
fi

# Copy in chunks
chunk_size=200000
offset=0
chunk_num=0

while [ $offset -lt "$filesize" ]; do
  dd if="$tempfile" bs=$chunk_size skip=$chunk_num count=1 2>/dev/null | pbcopy
  chunk_copied=$(dd if="$tempfile" bs=$chunk_size skip=$chunk_num count=1 2>/dev/null | wc -c | tr -d ' ')
  echo "Copied chunk of $chunk_copied characters to clipboard."
  if [ $((offset + chunk_size)) -lt "$filesize" ]; then
    echo "Press any key to copy the next chunk..."
    read -r -n 1 -s
  fi
  offset=$((offset + chunk_size))
  chunk_num=$((chunk_num + 1))
done

rm "$tempfile"
echo "All content has been copied to clipboard."