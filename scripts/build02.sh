#!/bin/bash
# compile code from root folder 
g++ -std=c++17 'programs/version2/main.cpp' 'programs/version2/controller.cpp' -o prog05_v2
# exectute the program from the root folder 
# $1 is the number of children 
# $2 is the path to the data folder 
# $3 is the path to the scrap folder 
./prog05_v2 $1 $2 $3

# Reomve the executable from the root folder 
rm prog05_v2
