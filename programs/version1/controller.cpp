#include "controller.hpp"

// function used determine if directory exists 
bool path_exist(const std::string &dpath){
    struct stat buffer;
    return (stat(dpath.c_str(), &buffer) == 0);
}

// make a new direcotry 
void make_dir(std::string dpath){
    int check = mkdir(dpath.c_str(), 0777);
    // validate that direcotry was actually made 
    if(!check){
        // std::cout<<"Srap folder created within 'programs/version1' \n";
    }else{
        std::cout<<"output folder was not created\n";
        exit(5);
    }
}

// clear directory 
void clear_dir(std::string dpath){
        // if directory exists, clear the contents of the direcoty 
        for (const auto& entry : std::filesystem::directory_iterator(dpath)){
            std::filesystem::remove_all(entry.path());
        } 
}

// count the number of files in a given directory
int file_count(std::string dpath){
    int counter = 0;
    std::filesystem::path path (dpath.c_str());
    for (auto& p : std::filesystem::directory_iterator(path))
        counter++;

    return counter;
}

// get all file names from a directory 
void file_names(std::string dpath, std::vector<std::string> *fnames){
    std::filesystem::path path (dpath.c_str());
    for(const auto & entry : std::filesystem::directory_iterator(path)){
        (*fnames).push_back(entry.path().filename());
    }
}

void split_work(std::vector<std::vector<std::string>> *assigned_work, 
std::vector<std::string> *fnames, int child_count){
    int f_count = fnames->size();
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
            (*assigned_work)[i].push_back((*fnames)[idx]);
            idx++;
        }
        // assign extra work if there is a remainder 
        if(extra_work > 0){
            (*assigned_work)[i].push_back((*fnames)[idx]);
            extra_work--;
            idx++;
        }
    }

}

void child_output(std::vector<std::string> *segment_of_work, int c_num, std::string o_path){
    // WRITE TO SCRAP FOLDER
    std::string child = "/child_";
    std::string num = std::to_string(c_num);
    std::string f_type = ".txt";
    std::string f_name = child + num + f_type;
    std::string output_path = o_path + f_name;
    std::ofstream CHILD(output_path);
    CHILD << "Child_"<<c_num<<" has recieved the following files:\n";
    for(int i = 0; i < segment_of_work->size(); i++){
        CHILD<<"\t"<<(*segment_of_work)[i]<<"\n";
    }
    CHILD.close();
    // STDOUT
    // std::cout<<"Child_"<<c_num<<" has recieved the following files:\n";
    // for(int i = 0; i < segment_of_work->size(); i++){
    //     std::cout<<"\t"<<(*segment_of_work)[i]<<"\n";
    // }
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

