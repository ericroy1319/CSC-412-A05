/* The first argument passed will be the number of child processes 
the parent proccess needs to create, the second argument passed will 
be the directory path to the data and the last argument passed will be 
the path to a scrap folder*/ 
#include "foo.cpp"
using namespace std;
int main(int argc, const char* argv[]){
    //-------------------------------------------------//
    //         Store Arguments into variables          //
    //-------------------------------------------------//
    /* It receives as arguments the number n of child 
    processes to create, the path to the data directory,
    and the path to a scrap folder.*/ 

    int child_count = std::stoi(argv[1]);
    std::string data_path = argv[2]; 
    std::string scrap_path = argv[3];
    scrap_path += "/scrap";

    //-------------------------------------------------//
    //             Create scrap folder                 //
    //-------------------------------------------------//
    /*The parent process should create the scrap folder if 
    it doesn’t already exist, and make sure that it 
    is empty otherwise.*/ 

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
    /* It splits the work into n more or less equal parts: 
    It determines that (future) Child k will look at files 
    from Index iinf to Index isup on the file list. */ 

    std::vector<std::string> f_names;
    file_names(data_path, &f_names);
    std::vector<std::string> f_paths;
    file_paths(data_path, &f_paths);
    std::vector<std::vector<std::string> > assigned_work;
    split_work(&assigned_work,&f_paths,child_count);   
    // print the result of the splitting of work 
    // print_split(&assigned_work);
    
    //-------------------------------------------------//
    //                  Create Pipes                   //
    //-------------------------------------------------//
    // fd[0] == reading end of pipe 
    // fd[1] == writing end of pipe
    // Parent to Child: ptc
    int fd_ptc[child_count+1][2];
    // Child to Parent: ctp 
    int fd_ctp[2];

    // open pipes 
    if(pipe(fd_ctp) == -1){
        cout<<"Pipe could not be created" << endl;
        return(50);
    }
    for(int i = 0; i <= child_count; i++){
        // create pipe 
        if(pipe(fd_ptc[i]) == -1){
           return(50);
        }
    }

    //-------------------------------------------------//
    //              Create Children                    //
    //-------------------------------------------------//
    /* It creates child processes to work on the files. 
    All child processes write one file named child <index>.txt 
    (e.g. child 1.txt,child 2.tx, etc.) into the scrap folder 
    (the content of the file is immaterial in this version)*/
    int pid = getpid();
    for(int i = 0; i < child_count; i++){
        if(pid != 0){
            pid = fork();
            if(pid ==0){
                // child to parent communication vector for redistribution of work
                std::vector<std::vector<std::string>> redistributed_work;
                // set the right size for first vector in 2D vector 
                for(int i = 0; i < child_count; i++){
                    std::vector<std::string> temp;
                    redistributed_work.push_back(temp);
                }
                determine_process_location(&assigned_work[i], &redistributed_work, i+1, child_count);
                child_output(&redistributed_work,scrap_path);
            }
        }
    }

    //-------------------------------------------------//
    //             Wait for Children                   //
    //-------------------------------------------------//
    /* The parent/dispatcher process waits for all child 
    processes to terminate,then writes an output file 
    in the scrap folder as well, then terminates. */ 
    int parent;
    while((parent=wait(NULL))>0);
    if(pid != 0){
        parent_output(scrap_path);
    }

    return 0;
}