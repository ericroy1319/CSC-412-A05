#include <iostream> 
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



// function used determine if directory exists 
bool path_exist(const std::string &str);

// make a new direcotry 
void make_dir(std::string path);

// clear directory 
void clear_dir(std::string path);

// count the number of files in a given directory
int file_count(std::string dir_path);

// get all file names from a directory 
void file_names(std::string dir_path, std::vector<std::string> *fnames);

void split_work(std::vector<std::vector<std::string>> *assigned_work, 
std::vector<std::string> *f_names, int child_count);

void child_output(std::vector<std::string> *segment_of_work, int c_num, std::string o_path);

void parent_output(std::string o_path);
