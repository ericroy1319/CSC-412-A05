#!/bin/bash
cd 'programs/version1/'
g++ -std=c++17 main.cpp -o prog
./prog
rm prog