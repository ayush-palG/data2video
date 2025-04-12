#!/bin/sh

set -xe

cc -Wall -Wextra -ggdb -fsanitize=address -I ./src -o main main.c
