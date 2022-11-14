#!/bin/bash
#temp=$(cut -d' ' -f1 $FILE)
c_num=0
# search the directory for all files of type .txt 
cd $1;
for FILE in *; do 
    temp=$(cut -b 1 $FILE)
    if [[ $temp -gt $c_num ]]; then 
        c_num=$temp;
    fi
done

echo "$c_num"