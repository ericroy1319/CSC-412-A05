#!/bin/bash

# Save the root folder path 
pwd=$(pwd)

# create variable to store child count 
c_num=0

# search the directory for all files of type .txt
#  navigate to directory 
cd $1;
# loop through directory and seach each file 
for FILE in *; do 
    # take the first byte from each file, will contain child process number 
    temp=$(cut -d' ' -f1 $FILE)
    # if the current process number is larger then child count, set c_cum to larger number 
    if [[ $temp -gt $c_num ]]; then 
        c_num=$temp;
    fi
done

# Navigate back to root folder 
cd "$pwd"
c_num=$((c_num+1))
# call build 
# c_num is the number of children 
# $1 is the path to the data folder 
# $2 is the path to the scrap folder 
./scripts/build01.sh $c_num $1 $2
