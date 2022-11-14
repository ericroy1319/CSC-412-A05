#!/bin/bash
# compile code from root folder 
g++ -std=c++17 'programs/version1/main.cpp' -o prog
# exectute the program from the root folder 
./prog 2 "/Users/ericroy/Documents/uri/Fall_2022/CSC-412/Data-Sets/Large-Data-Sets/DataSet-1--2-procs" "programs/version1"

# Reomve the executable from the root folder 
rm prog
