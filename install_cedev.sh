#!/bin/sh
set -eu

CEDEV_URL="https://github.com/CE-Programming/toolchain/releases/download/v14.2/CEdev-Linux.tar.gz"
ARCHIVE="CEdev-Linux.tar.gz"

if [ -d "$PWD/CEdev" ]; then
  echo "Skipping CDdev installation - already installed in tree"
else
  rm -rf CEdev "$ARCHIVE"
  wget -O "$ARCHIVE" "$CEDEV_URL"
  tar xzf "$ARCHIVE"
  rm -f "$ARCHIVE"
fi

./CEdev/bin/ez80-clang --version

export PATH="$PWD/CEdev/bin:$PATH"
make -f Makefile.ti84ce