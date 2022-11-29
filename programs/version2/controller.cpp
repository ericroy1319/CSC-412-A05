#include "controller.hpp"
using namespace std;

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
        return;
    }else{
        std::cout<<"Output folder was not created\n";
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
    std::filesystem::path dir_path (dpath.c_str());
    for (auto& p : std::filesystem::directory_iterator(dir_path))
        counter++;

    return counter;
}

// get all file names from a directory 
void file_names(std::string dpath, std::vector<std::string> *fnames){
    std::filesystem::path dir_path (dpath.c_str());
    for(const auto & entry : std::filesystem::directory_iterator(dir_path)){
        if(entry.is_regular_file()){
            if(entry.path().extension().string() == ".txt"){
                (*fnames).push_back(entry.path().filename());
            }
        }
    }
}

// get all the file paths from a directory 
void file_paths(std::string dpath, std::vector<std::string> *fpaths){
    std::filesystem::path dir_path (dpath.c_str());
    for(const auto & entry : std::filesystem::directory_iterator(dir_path)){
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
    // std::cout<<"\t" << str<<std::endl;
}

// function used to read message from child 
/// NOT CURRENTLY USED
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
// First Generation of Child Output  
void first_gen_child_work(std::vector<std::string>* fpaths, std::string* write_to_path, int* c_num){
    // WRITE TO SCRAP FOLDER
    int len = fpaths->size();
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
        std::ofstream FILE(outputPath);
        // CHILD << "Child_"<<c_num<<" is assigned the following files:\n";
        for(int i = 0; i < len; i++){
            FILE<<(*fpaths)[i]<<"\n";
        }
        FILE.close();
    }else{
        std::ofstream FILE;
        FILE.open(outputPath, std::ios_base::app);
        for(int i = 0; i < len;i++){
            FILE<<(*fpaths)[i]<<"\n";
        }
        FILE.close();
    }
}

void get_content(std::string fpath, std::string* content){
    std::ifstream file;
    file.open(fpath);
    if(file.is_open() == false){
        cout << "ERROR" << endl;
    }
    std::string line; getline(file,line);
    (*content) = line;
    file.close();
}

// Second Gerneration of Child Output 
void second_gen_child_work(std::string scrap_path, std::string fpath){
    // open file and read content, write to path 
    std::ifstream file;
    std::string content;
    std::string child;
    std::string fileType = ".txt";
    std::string fileName;
    std::string outputPath;

    file.open(fpath);
    // iterate through file and get each line
    bool execute_code_block = true;
    for(string line; getline(file,line);){
        // pull the file path from the file 
        // cout << line << endl;
        get_content(line, &content);
        if(execute_code_block == true){
            string c_num; 
            char ch; 
            for(int c = 0; c < content.size(); c++){
                ch = content[c];
                if(ch == ' '){
                    execute_code_block = false;
                    break;
                }else{
                    c_num += ch;
                }
            }
            child = "/child_" + c_num + "_fileContent";
            fileName = child+fileType;
            outputPath = scrap_path + fileName;
        }
        // WRITE TO SCRAP FOLDER
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
            std::ofstream FILE(outputPath);
            if(FILE.is_open() == false){
                cout << "FILE NOT OPEN\t" << outputPath <<  endl;
            }
            // cout << "[DEBUG] Writing Content " << content << endl;
            FILE << content << "\n";
            FILE.close();
        }else{
            std::ofstream FILE;
            FILE.open(outputPath, std::ios_base::app);
            FILE << content << "\n";
            // cout << "[DEBUG] Writing Content " << content << endl;
            FILE.close();
        }    
    }
}    

void determine_process_location(std::string *fpath, std::string *send_to){
    /* loop through assigned segement of files, determine where they need go,
    place them into the correct index of redistributed_work vector, and this 
    will be used to pipe to parent */
    std::ifstream file; 
    // open file from filepath 
    file.open(*fpath);
    // get the first line from the file 
    std::string line; getline(file,line);
    // seperate the line by white space 
    std::stringstream s(line);
    // store string segment into process
    // the first segment will tell you correct child process 
    s >> (*send_to);
    file.close();
}

void read_fileContent(std::string fpath, std::vector<std::string>* content){
    std::ifstream file;
    file.open(fpath);
    if(file.is_open() == false){
        cout << "ERROR" << endl;
    }
    for(std::string line; getline(file,line);){
        std::string temp = line;
        (*content).push_back(temp);
    }
    file.close();
}

void parent_output(std::string scrap_path){
    
    // Path for file to be written 
    std::string parent = "/parent";
    std::string f_type = ".txt";
    std::string f_name = parent + f_type;
    std::string output_path = scrap_path + f_name;
    // std::ofstream PARENT(output_path);
    // get all the files from the scrap directory 
    std::vector<std::string> fpaths;
    file_paths(scrap_path, &fpaths);
    // for(int i = 0; i < fpaths.size(); i ++){
    //     cout << "\t"<<fpaths[i] << endl;
    // }
    // every odd index will be a fileContent path 
    std::vector<std::vector<std::string> > fileContent; 
    for(int i = 0; i < fpaths.size(); i++){
        std::vector<std::string> temp;
        // cout << "[DEBUG] fpath " << i << " " << fpaths[i] << endl;
        read_fileContent(fpaths[i], &temp);

        char ch = temp[0][0];
        if(isdigit(ch)==true){
            fileContent.push_back(temp);
        }
        
    }
    // print_fileContent(fileContent);
    std::ofstream PARENT(output_path);
    for(int i = 0; i < fileContent.size(); i++){
        for(int j = 0; j < fileContent[i].size(); j++){
            PARENT << fileContent[i][j] << "\n";
        }
    }
    PARENT.close();
}
