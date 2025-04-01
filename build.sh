#!/bin/sh

set -xe

cc -Wall -Wextra -ggdb -fsanitize=address -o main main.c
