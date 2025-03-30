#!/usr/bin/env sh

mkdir -p bin/

rm -f bin/*

flags="-lraylib -lGL -lm"

gcc -o bin/pp_model pp_model.c $flags
gcc -o bin/pm_model pm_model.c $flags

