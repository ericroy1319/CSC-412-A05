/**
 * @file foo.hpp 
 * This header file declares all of the functions used by main.cpp
 */

#include <iostream> 
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <cstdlib>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/// Global variable to control the size of a message sent over a pipe
#define BUFF_SIZE 256
/// Global variable to simplify using the write end of a pipe
#define WRITE 1
/// Global variable to simplify using the read end of a pipe
#define READ 0
/// Global variable used to signal that no more information will be send by process over pipe
#define TERMINATE 'T'
/// Global variable used to signal that there is information being send over the pipe by a process 
#define EXECUTE 'R' 
// Global variable used to simplify the determination of being within a child process or not 
#define CHILD 0
// Global variable used to simplify creation of a pipe array of [2] read and write 
#define PIPE 2
// Global varaible use to simplify the search for seperation of content within a message sent over a pipe
#define SPACE ' '

/// @brief tests to see if provided directory path points to a direcotry or not 
/// @param dpath is the directory path to test 
/// @return Returns true if direcotry exists and false if not  
bool path_exist(const std::string &path);

/// @brief will make a new direcotry at a given location
/// @param dpath is the given location to make the directory
void make_dir(std::string path);

/// @brief will clear all contents of a directory at a given location 
/// @param dpath is the given location for the function to clear 
void clear_dir(std::string path);

/// @brief will count the number of files in a given directory 
/// @param dpath the path the the directory to count it's file contents 
/// @return the number of files within the directory 
int file_count(std::string path);

/// @brief will get all of the .txt files with in the directory
/// @param dpath is the path to the directory to search
/// @param fnames pointer to a data structure to hold the file names
void file_names(std::string path, std::vector<std::string> *fnames);

/// @brief will get all of the .txt filepaths within a given directory 
/// @param dpath the path to the directory to search
/// @param fpaths pointer to a data structure to hold the file paths 
void file_paths(std::string path, std::vector<std::string> *fpaths);

/// @brief used to convert a char[] array into a string 
/// @param str pointer to a string to write to 
/// @param arr pointer to a char[] array to read from 
/// @param len length of the char[] array 
void convert_to_string(std::string* str, char* arr, int len);

/// @brief is used to seperate the files as close to even as possible to each child
/// @param assigned_work pointer to a 2D Vector of Strings to hold the file paths 
/// @param fnames pointer to the data stucutre that hold the list of file names, used for size
/// @param child_count the number of child process that the work will be distributed to 
void split_work(std::vector<std::vector<std::string>> *assigned_work, 
std::vector<std::string> *fnames, int child_count);

/// @brief opens files from file path and determines which process it belongs to
/// @param fpath the file path to to read from 
/// @param send_to the child process number (PID)
void determine_process_location(std::string *fpath, std::string *send_to);

/// @brief 1st gen of child processes work 
/// @param fpaths the file path to read from 
/// @param write_to_path the file path to write to 
/// @param c_num the current process number (PID)
void first_gen_child_work(std::vector<std::string>* fpaths, std::string* write_to_path, int* c_num);

/// @brief 2nd gen of child processes work 
/// @param scrap_path the file path to the scrap folder 
/// @param fpath the file path to read from 
void second_gen_child_work(std::string scrap_path, std::string fpath);

/// @brief used by second_gen_child_work to read content from a file
/// @param fpath the path to the file 
/// @param content a pointer to a data structure to store the content of the file 
void get_content(std::string fpath, std::string* content);


/// @brief used by parent output to read file content from a child process output file
/// @param fpath fthe to the file to read from 
/// @param content a pointer to the data structure to hold the content of the file 
void read_fileContent(std::string fpath, std::vector<std::string>* content);

/// @brief work done by the parent after both gens of child processes finish
/// @param scrap_path path to the scrap fold to write to 
void parent_output(std::string scrap_path);

