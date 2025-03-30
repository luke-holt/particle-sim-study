#!/usr/bin/env sh

mkdir -p bin/

rm -f bin/*

flags="-lraylib -lGL -lm"

gcc -o bin/basic_pp_model basic_pp_model.c $flags
gcc -o bin/basic_pm_model basic_pm_model.c $flags

