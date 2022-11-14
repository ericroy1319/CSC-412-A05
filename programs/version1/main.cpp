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
    std::vector<std::vector<std::string> > assigned_work;
    split_work(&assigned_work,&f_names,child_count);   
    // print the result of the splitting of work 
    print_initial_split(&assigned_work);

    //-------------------------------------------------//
    //              Create Children                    //
    //-------------------------------------------------//

    return 0;
}