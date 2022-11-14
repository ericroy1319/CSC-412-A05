#!/bin/bash
# compile code from root folder 
g++ -std=c++17 'programs/version1/main.cpp' -o prog
# exectute the program from the root folder 
./prog 4 "/Users/ericroy/Documents/uri/Fall_2022/CSC-412/Data-Sets/Data-Set-1" "programs/version1"

# Reomve the executable from the root folder 
rm prog
# Remove the scrap directory from the programs/version1 directory
# rmdir 'programs/version1/scrap'