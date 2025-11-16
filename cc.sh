#!/bin/bash
set -e

COMPILER=gcc
CXXFLAGS="-Wall -Wextra -Werror -g -O3"
LDFLAGS="-lcurl"

SRC_DIR=src
BUILD_DIR=build
OUTPUT=$BUILD_DIR/nett.elf

mkdir -p $BUILD_DIR

OBJ_FILES=""
PIDS=()
HEADERS=$(find $SRC_DIR -name '*.h')

for file in $(find $SRC_DIR -name '*.c'); do
  obj_file="$BUILD_DIR/$(basename "${file%.c}.o")"
  OBJ_FILES="$OBJ_FILES $obj_file"

  needs_recompile=false

  if [[ ! -f $obj_file || $file -nt $obj_file ]]; then
    needs_recompile=true
  else
    for header in $HEADERS; do
      if [[ $header -nt $obj_file ]]; then
        needs_recompile=true
        break
      fi
    done
  fi

  if $needs_recompile; then
    echo "🔄 Compiling.. $file"
    $COMPILER -c $CXXFLAGS "$file" -o "$obj_file" &
    PIDS+=($!)
  fi
done

for pid in "${PIDS[@]}"; do
  wait $pid
done

echo "🔗 Linking... $OUTPUT"
$COMPILER $OBJ_FILES -o $OUTPUT $LDFLAGS

echo "✅ Build made: $OUTPUT"
