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
bool path_exist(const std::string &str){
    struct stat buffer;
    return (stat(str.c_str(), &buffer) == 0);
}

// make a new direcotry 
void make_dir(std::string path){
    int check = mkdir(path.c_str(), 0777);
    // validate that direcotry was actually made 
    if(!check){
        std::cout<<"Srap folder created within 'programs/version1' \n";
    }else{
        std::cout<<"Scrap folder was not created\n";
        exit(5);
    }
}

// clear directory 
void clear_dir(std::string path){
        // if directory exists, clear the contents of the direcoty 
        for (const auto& entry : std::filesystem::directory_iterator(path)){
            std::filesystem::remove_all(entry.path());
        } 
}

// count the number of files in a given directory
int file_count(std::string dir_path){
    int counter = 0;
    std::filesystem::path path (dir_path.c_str());
    for (auto& p : std::filesystem::directory_iterator(path))
        counter++;

    return counter;
}

// get all file names from a directory 
void file_names(std::string dir_path, std::vector<std::string> *fnames){
    std::filesystem::path path (dir_path.c_str());
    for(const auto & entry : std::filesystem::directory_iterator(path)){
        (*fnames).push_back(entry.path().filename());
    }
}

void split_work(std::vector<std::vector<std::string>> *assigned_work, 
std::vector<std::string> *f_names, int child_count){
    int f_count = f_names->size();
    int split = f_count / child_count;
    int extra_work = f_count % child_count;
    int idx = 0; 
    std::cout << "\t split == "<< split << std::endl;
    std::cout << "\t extra_work == "<< extra_work << std::endl;
    for(int i = 0; i < child_count; i++){
        int j;
        std::vector<std::string> temp;
        (*assigned_work).push_back(temp);
        for(j = 0 ; j < split; j ++){
            // add the normal work load for child 
            (*assigned_work)[i].push_back((*f_names)[idx]);
            idx++;
        }
        // assign extra work if there is a remainder 
        if(extra_work > 0){
            (*assigned_work)[i].push_back((*f_names)[idx]);
            extra_work--;
            idx++;
        }
    }

}

// print function for showing what files intially go to which children 
void print_initial_split(std::vector<std::vector<std::string>> *the_split){
    std::cout<<std::endl;
    for(int i = 0; i < (*the_split).size(); i++){
        std::cout<<"Child <"<<i+1 << "> will get: \t";
        for(int j = 0; j < (*the_split)[i].size(); j++){
            
            std::cout << (*the_split)[i][j] << " ";
        }
        std::cout<<"\n";
    }

}