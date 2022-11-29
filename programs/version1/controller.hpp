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



/// @brief tests to see if provided directory path points to a direcotry or not 
/// @param dpath is the directory path to test 
/// @return Returns true if direcotry exists and false if not  
bool path_exist(const std::string &dpath);

/// @brief will make a new direcotry at a given location
/// @param dpath is the given location to make the directory
void make_dir(std::string dpath);

/// @brief will clear all contents of a directory at a given location 
/// @param dpath is the given location for the function to clear 
void clear_dir(std::string dpath);

/// @brief will count the number of files in a given directory 
/// @param dpath the path the the directory to count it's file contents 
/// @return the number of files within the directory 
int file_count(std::string dpath);

/// @brief will get all of the .txt files with in the directory
/// @param dpath is the path to the directory to search
/// @param fnames pointer to a data structure to hold the file names
void file_names(std::string dpath, std::vector<std::string> *fnames);

/// @brief is used to seperate the files as close to even as possible to each child
/// @param assigned_work pointer to a 2D Vector of Strings to hold the file paths 
/// @param fnames pointer to the data stucutre that hold the list of file names, used for size
/// @param child_count the number of child process that the work will be distributed to 
void split_work(std::vector<std::vector<std::string>> *assigned_work, 
std::vector<std::string> *fnames, int child_count);

/// @brief is used to output the recieved files to an output file in the scrap directory 
/// @param segment_of_work data structure that hold the initial files sent to process
/// @param c_num used for writing the child number to file 
/// @param o_path the path to the scrap folder 
void child_output(std::vector<std::string> *segment_of_work, int c_num, std::string o_path);

/// @brief work done by the parent after both gens of child processes finish
/// @param o_path path to the scrap fold to write to 
void parent_output(std::string scrap_path);