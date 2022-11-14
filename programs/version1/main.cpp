/* The first argument passed will be the number of child processes 
the parent proccess needs to create, the second argument passed will 
be the directory path to the data and the last argument passed will be 
the path to a scrap folder*/ 
#include "foo.cpp"

int main(int argc, const char* argv[]){
    //-------------------------------------------------//
    //         Store Arguments into variables          //
    //-------------------------------------------------//
    int child_count = std::stoi(argv[1]);
    std::string data_path = argv[2]; 
    std::string scrap_path = argv[3];
    scrap_path += "/scrap";

    //-------------------------------------------------//
    //             Create scrap folder                 //
    //-------------------------------------------------//
    // check to see if directory exists 
    bool is_there = path_exist(scrap_path);
    if(is_there == false){
        make_dir(scrap_path);
    }
    else{
        clear_dir(scrap_path);
    }

    //-------------------------------------------------//
    //             Split the work                      //
    //-------------------------------------------------//
    /* Take the number of files, and the number of child process
    and split the work as close to evenly as possible*/ 
    std::vector<std::string> f_names;
    file_names(data_path, &f_names);
    int f_count = f_names.size();
    // divide the work 
    int split = f_count / child_count;
    std::cout << "\t split == "<< split << std::endl;
    // check if there is an even split, store remainder 
    int extra_work = f_count % child_count;
    std::cout << "\t extra_work == "<< extra_work << std::endl;

    // the index of the 1st vector + 1 is the child number 
    // the nested vector hold the files the child is assigned to work on 
    std::vector<std::vector<std::string> > assigned_work;
    
    int idx = 0; 
    for(int i = 0; i < child_count; i++){
        int j;
        std::vector<std::string> temp;
        assigned_work.push_back(temp);
        for(j = 0 ; j < split; j ++){
            // add the normal work load for child 
            assigned_work[i].push_back(f_names[idx]);
            idx++;
        }
        // assign extra work if there is a remainder 
        if(extra_work > 0){
            assigned_work[i].push_back(f_names[idx]);
            extra_work--;
            idx++;
        }
    }
    std::cout<<std::endl;
    for(int i = 0; i < assigned_work.size(); i++){
        std::cout<<"Child <"<<i+1 << "> will get: \t";
        for(int j = 0; j < assigned_work[i].size(); j++){
            
            std::cout << assigned_work[i][j] << " ";
        }
        std::cout<<"\n";
    }


    return 0;
}