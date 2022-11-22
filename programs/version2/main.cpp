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
  
    int terminate_IPC = child_count; 
    int pid = getpid();
    for(int i = 0; i < child_count; i++){
        if(pid != 0){
            pid = fork();
            if(pid ==0){
                  cout << "[DEBUG] child_" << i << " initially recieves " << assigned_work[i].size() << " files to process" << endl;
                // cout << "\tChild_"<<i<<" created" << endl;
                // read from assigned segment of work and determine what need 
                // to go back to the parent via pipe for redistribution 
                std::string send_to_process;
                std::vector<std::string> process_files;
                close(fd_ptc[i][1]);
                close(fd_ctp[0]);
                //******************************//
                //     Write to Parent Pipe     // 
                //******************************//
                
                for(int c = 0; c < assigned_work[i].size(); c++){
                    
                    // extract the correct process location from the file 
                    determine_process_location(&assigned_work[i][c], &send_to_process);
                    // if file does not belong to self for processing, pass it back to parent
                    
                    if(stoi(send_to_process) != i){
                    //    cout << "[DEBUG] " << i << " is NOT keeping " <<  assigned_work[i][c] << endl;
                        // cout << "current file belongs too " << i << " need to go to "<<send_to_process << endl;
                        // attach correct process to front of file path 
                        std::string message = send_to_process + " " + assigned_work[i][c];
                        
                        // write to parent 
                        // get legnth of file path 
                        int message_len = message.size();
         
                        // create a char array to hold file path 
                        char out_message[message_len + 1];
                        
                        // copy string to charcter array 
                        stpcpy(out_message, message.c_str());
                 
                        // cout << "\tChild_"<<i<<" writing to parent:  " << out_message << endl;
                        // first send the lenght of the message to the parent 
                        write(fd_ctp[1], &message_len, sizeof(int));
                        // next send the message to the parent 
                        write(fd_ctp[1], &out_message, message_len+1);
                    }else{
                        // if file does belong to self, save for processing 
                        // add file path to keep list for processing
                        // cout << "[DEBUG] " << i << " is keeping " <<  assigned_work[i][c] << endl;
                        process_files.push_back(assigned_work[i][c]); 
                    }
                }
                // once all of the messages have been send, send terminating message
                int terminator = -1;
                write(fd_ctp[1], &terminator, sizeof(int));
                // cout << "[DEBUG] Terminator should be sent twice " <<  endl;
                // close writing end of pipe 
                if(close(fd_ctp[1]) == -1){
                    cout << "ERROR" << endl;
                };

                //******************************//
                //     Read From Child Pipe     // 
                //******************************//
                // cout << "\tChild_"<<i<<" reading from parent" << endl;
                bool keepReading = true;
               
                while(keepReading == true){
                    // cout << "[DEBUG] child_" << i << "  READIING FROM PARENT"<<endl;
                    int message_len;
                    // get the length of the inbound message 
                    // cout << "[DEBUG] HERE" << endl;
                    read(fd_ptc[i][0], &message_len, sizeof(int));
                    // cout <<"[DEBUG] " << message_len << endl;
                    if(message_len < 0){
                        keepReading = false;
                        // close parent to child pipe [reading]
                        close(fd_ptc[i][0]);
                        //  cout << "[DEBUG] child_" << i << "  STOP READING FROM PARENT"<<endl;;
                        break;
                    }else{
                        // create a container for the inbound message 
                        
                        char in_message[message_len+1];
                        // read the inbound message 
                        read(fd_ptc[i][0], &in_message, message_len+1);
                        // cout << "[DEBUG] " << in_message << endl;
                        // store message in process file vector 
                        std::string message;
                        for(int c = 0; c < message_len; c++){
                            message += in_message[c];
                        }
                        // cout << "\tChild_"<<i<<" has recieved value: "<< message << " from parent!"<<endl;
                        process_files.push_back(message);
                    }
                }
                cout << "[DEBUG] child_" << i << " will process " << process_files.size() << " files: " << endl;
                // function used to output to scrap folder 
                child_output(&process_files, &scrap_path, &i);
                return 0;
            }else{
                //******************************//
                //     Read from Parent Pipe    // 
                //******************************//
                //  cout << "Parent is reading from pipes"<<endl;
                // incoming message:  <process # > < " " > < file_path > 
                bool keepReading = true;
                while(keepReading == true){
                    int message_len;
                    // read in message length from child 
                    read(fd_ctp[0], &message_len, sizeof(int));
                    if(message_len == -1){
                        // cout << "Parent recieved terminating code" << endl;
                        terminate_IPC--;
                        keepReading=false;
                        break;
                    }else{
                        // container to store message
                        char in_message[message_len+1];
                        // read in message from child 
                        read(fd_ctp[0], &in_message, message_len+1);
                        char ch;
                        int idx=0;
                        bool is_true = true;
                        std::string destination;
                        std::string fpath;
                        // cout << " in_message ==" << in_message <<endl;
                        while(is_true == true){
                            ch = in_message[idx];
                            if(ch == ' '){
                                idx++; 
                                is_true = false;
                                break;
                            }else{
                                destination += ch; 
                                idx++;
                            }
                        }
                        // cout << "The idx should be 2 " << idx << endl;
                        is_true = true;
                        while(is_true == true){
                           
                            ch = in_message[idx];
                            // cout << ch << endl;
                            if(ch == '\0'){
                                
                                is_true = false;
                                break;
                            }else{
                                fpath += ch;
                                idx++;
                            }
                        }

                        //******************************//
                        //    Write to Children Pipes   // 
                        //******************************//
                        cout << "Parent will send to Child_" << destination << " the path: " << fpath << endl;
                        char message[fpath.size()+1];
                        strcpy(message, fpath.c_str());
                        write(fd_ptc[stoi(destination)][1], &message_len, sizeof(int));
                        write(fd_ptc[stoi(destination)][1], &message, message_len+1);
                    }
                }
                if(terminate_IPC == 0) {
                    // send terminator value to all children 
                    int end = -1;
                    for(int k = 0; k < child_count; k++){
                        write(fd_ptc[k][1], &end, sizeof(int));
                        // close parent to child pipe [write] 
                        close(fd_ptc[k][1]);
                    }
                }
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
        cout << "All Children Processes Are Terminated" << endl;
        parent_output(scrap_path);
    }

    return 0;
}