#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")"
IPLAY_LAUNCHER_DISPLAY=./iplay.sh exec ./rewrite/iplay.sh "$@"
