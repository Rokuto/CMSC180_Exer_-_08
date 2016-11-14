#!/bin/bash

rm output.txt

mpicc -o hello.exe hello.c

sbatch submit.sh ./hello.exe 1

