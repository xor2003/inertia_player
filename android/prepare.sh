#!/bin/sh
set -eu
cd "$(dirname "$0")"

mkdir -p vendor app/src/main/sdl-java app/src/main/assets

if [ ! -d vendor/SDL/.git ]; then
  git clone --depth 1 --branch release-2.32.10 https://github.com/libsdl-org/SDL.git vendor/SDL
fi
if [ ! -d vendor/libxmp/.git ]; then
  git clone --depth 1 --branch libxmp-4.6.3 https://github.com/libxmp/libxmp.git vendor/libxmp
fi

cp -R vendor/SDL/android-project/app/src/main/java/org app/src/main/sdl-java/
cp ../samples/HACKER4.S3M app/src/main/assets/HACKER4.S3M
cp ../iplay.png app/src/main/assets/iplay.png
