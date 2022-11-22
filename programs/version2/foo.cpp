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
        return;
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
        if(entry.is_regular_file()){
            if(entry.path().extension().string() == ".txt"){
                (*fnames).push_back(entry.path().filename());
            }
        }
    }
}

// get all the file paths from a directory 
void file_paths(std::string dir_path, std::vector<std::string> *fpaths){
    std::filesystem::path path (dir_path.c_str());
    for(const auto & entry : std::filesystem::directory_iterator(path)){
        if(entry.is_regular_file()){
            if(entry.path().extension().string() == ".txt"){
                (*fpaths).push_back(entry.path());
            }
        }
    }
}

// convert a charcter array to a string
void convert_to_string(std::string* str, char* arr, int len){
    for(int c = 0; c < len; c++){
        (*str) = (*str) + arr[c];
    }
    std::cout<<"\t" << str<<std::endl;
}

// function used to read message from child 
void parse_message(char* arr, int len, std::string* send_to, std::string* fpath){
    char ch;
    int idx;
    bool is_true = true;
    while(is_true == true){
        ch = arr[idx];
        if(ch == ' '){
            idx++; 
            is_true = false;
            break;
        }else{
            *send_to+= ch; 
            idx++;
        }
    }
    is_true = true;
    while(is_true == true){
        ch = arr[idx];
        if(ch == '\0'){
            is_true = false;
            break;
        }else{
            fpath += ch;
            idx++;
        }
    }
}

void split_work(std::vector<std::vector<std::string>> *assigned_work, 
std::vector<std::string> *f_names, int child_count){
    int f_count = f_names->size();
    int split = f_count / child_count;
    int extra_work = f_count % child_count;
    int idx = 0; 
    // std::cout << "\t split == "<< split << std::endl;
    // std::cout << "\t extra_work == "<< extra_work << std::endl;
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

void child_output(std::vector<std::string>* file_paths, std::string* write_to_path, int* c_num){
    // WRITE TO SCRAP FOLDER
    int len = file_paths->size();
    std::string child = "/child_" + std::to_string(*c_num);
    std::string fileType = ".txt";
    std::string fileName = child+fileType;
    std::string outputPath = *write_to_path + fileName;

    // check to see if file exists 
    bool file_exists;
    struct stat buffer;
    if(stat(outputPath.c_str(),&buffer) != -1){
        file_exists = true;
    }else{
        file_exists = false;
    }
    // if file does not exist create and write to it 
    if(file_exists == false){
        std::ofstream CHILD(outputPath);
        CHILD << "Child_"<<c_num<<" is assigned the following files:\n";
        for(int i = 0; i < len; i++){
            CHILD<<"\t"<<(*file_paths)[i]<<"\n";
        }
        CHILD.close();
    }else{
        std::ofstream CHILD;
        CHILD.open(outputPath, std::ios_base::app);
        for(int i = 0; i < len;i++){
            CHILD<<"\t"<<(*file_paths)[i]<<"\n";
        }
        CHILD.close();
    }
}
    
void determine_process_location(std::string *file_path, std::string *send_to){
    /* loop through assigned segement of files, determine where they need go,
    place them into the correct index of redistributed_work vector, and this 
    will be used to pipe to parent */
    std::ifstream file; 
    // open file from filepath 
    file.open(*file_path);
    // get the first line from the file 
    std::string line; getline(file,line);
    // seperate the line by white space 
    std::stringstream s(line);
    // store string segment into process
    // the first segment will tell you correct child process 
    s >> (*send_to);
}

void parent_output(std::string o_path){
    // WRITE TO SCRAP FOLDER
    std::string parent = "/parent";
    std::string f_type = ".txt";
    std::string f_name = parent + f_type;
    std::string output_path = o_path + f_name;
    std::ofstream PARENT(output_path);
    PARENT << "This is the parent output file\n";
    PARENT.close();
}

// print function for showing what files intially go to which children 
void print_split(std::vector<std::vector<std::string>> *the_split){
    std::cout<<std::endl;
    for(int i = 0; i < (*the_split).size(); i++){
        std::cout<<"Child <"<<i+1 << "> will get: \t";
        for(int j = 0; j < (*the_split)[i].size(); j++){
            
            std::cout << (*the_split)[i][j] << " ";
        }
        std::cout<<"\n";
    }

}