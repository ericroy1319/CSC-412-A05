/* The first argument passed will be the number of child processes 
the parent proccess needs to create, the second argument passed will 
be the directory path to the data and the last argument passed will be 
the path to a scrap folder*/ 
#include "foo.cpp"
using namespace std;
#define BUFF_SIZE 1024

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
    //            Create Children Gen 1                //
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
                
                // cout << "[DEBUG] child_" << i << " initially recieves " << assigned_work[i].size() << " files to process" << endl;
                // cout << "\tChild_"<<i<<" created" << endl;
                // read from assigned segment of work and determine what need 
                // to go back to the parent via pipe for redistribution 
                std::string send_to_process;
                std::vector<std::string> process_files;
                if(close(fd_ptc[i][1]) == -1){
                    cout << "ERROR 1" << endl;
                };
                if(close(fd_ctp[0]) == -1){
                    cout << "ERROR 2" << endl;
                };
    
                //******************************//
                //     Write to Parent Pipe     // 
                //******************************//
                
                for(int c = 0; c < assigned_work[i].size(); c++){
                    
                    // extract the correct process location from the file 
                    determine_process_location(&assigned_work[i][c], &send_to_process);
                    // if file does not belong to self for processing, pass it back to parent
                    
                    if(stoi(send_to_process) != i){
                        // <ACTION><PID><MESSAGE_LENGTH><MESSAGE>
                        char action = 'R';
                        // get legnth of file path 
                        // cout << "[DEBUG] child_" << i << " is sending out " << assigned_work[i][c] <<endl;
                        int message_len = assigned_work[i][c].size();
                        // attach correct process to front of file path 
                        std::string message = action + send_to_process + " " + std::to_string(message_len) + " " + assigned_work[i][c];
                    
                        // create a char array to hold file path 
                        char out_message[BUFF_SIZE];
                        
                        // copy string to charcter array 
                        stpcpy(out_message, message.c_str());
                        
                        // write to parent 
                        write(fd_ctp[1], &out_message, BUFF_SIZE);
                        // cout << "[DEBUG] child_" << i << " is sending out " << out_message <<endl;

                    }else{
                        // if file does belong to self, save for processing 
                        // cout << "[DEBUG] child_" << i << " is holding onto " << assigned_work[i][c] <<endl;
                        process_files.push_back(assigned_work[i][c]); 
                    }
                }
                // once all of the messages have been send, send terminating message
                char terminator = 'T';
                write(fd_ctp[1], &terminator, BUFF_SIZE);
                // cout << "[DEBUG] child_" << i << " is sending out " << terminator <<endl;
                // close writing end of pipe 
                if(close(fd_ctp[1]) == -1){
                    cout << "ERROR 3" << endl;
                    exit(50);
                };

                //******************************//
                //     Read From Child Pipe     // 
                //******************************//
                bool keepReading = true;
               
                while(keepReading == true){
                    
                    char read_pipe[BUFF_SIZE];
                    read(fd_ptc[i][0], &read_pipe, BUFF_SIZE);
                    // cout << "[DEBUG] child_" << i << " is reading " << read_pipe <<endl;

                    // check for terminating value 
                    if(read_pipe[0] == 'T'){
                        // cout << "[DEBUG] child_" << i << " has recieved the terminator" <<endl;
                        keepReading = false;
                        // close parent to child pipe [reading]
                        close(fd_ptc[i][0]);
                        break;
                    }else{
                        // <ACTION><PID><MESSAGE_LENGTH><MESSAGE>
                        // index to keep track where we are in pipe message
                        int idx = 1; 
                        // charcter used to read message 
                        char ch;
                        bool continueReading = true;
                        // extract destination process 
                        std::string c_num;
                        while(continueReading == true){
                            ch = read_pipe[idx];
                            if(ch == ' '){
                                idx++;
                                continueReading = false;
                                break;
                            }else{
                                c_num += ch;
                                idx++;
                            }
                        }
                        // extract the message length 
                        // reset loop control
                        continueReading = true;
                        std::string message_len;
                        while(continueReading == true){
                            ch = read_pipe[idx];
                            if(ch == ' '){
                                idx++;
                                continueReading = false;
                                break;
                            }else{
                                message_len += ch;
                                idx++;
                            }
                        }
                        // extract message 
                        std::string message;
                        int len = stoi(message_len);
                        for(int c = 0; c < len; c++){
                            ch = read_pipe[idx];
                            message += ch;
                            idx++;
                        }
                        process_files.push_back(message);
                    }
                }
                // cout << "[DEBUG] child_" << i << " will process " << process_files.size() << " files: " << endl;
                // function used to output to scrap folder 
                child_output(&process_files, &scrap_path, &i);
                return 0;
            }else{
                //******************************//
                //     Read from Parent Pipe    // 
                //******************************//
                bool keepReading = true;
                while(keepReading == true){
                    // <ACTION><PID><MESSAGE_LENGTH><MESSAGE>
                    char read_pipe[BUFF_SIZE];
                    read(fd_ctp[0], &read_pipe, BUFF_SIZE);

                    // check for terminating value 
                    if(read_pipe[0] == 'T'){
                        keepReading = false;
                        terminate_IPC--;
                        break;
                    }else{
                        // <ACTION><PID><MESSAGE_LENGTH><MESSAGE>
                        // index to keep track where we are in pipe message
                        int idx = 1; 
                        // charcter used to read message 
                        char ch;
                        bool continueReading = true;
                        // extract destination process 
                        std::string destination;
                        while(continueReading == true){
                            ch = read_pipe[idx];
                            if(ch == ' '){
                                idx++;
                                continueReading = false;
                                break;
                            }else{
                                destination += ch;
                                idx++;
                            }
                        }
                        // extract the message length 
                        // reset loop control
                        continueReading = true;
                        std::string message_len;
                        while(continueReading == true){
                            ch = read_pipe[idx];
                            if(ch == ' '){
                                idx++;
                                continueReading = false;
                                break;
                            }else{
                                message_len += ch;
                                idx++;
                            }
                        }
                        // extract message 
                        std::string message;
                        int len = stoi(message_len);
                        for(int c = 0; c < len; c++){
                            ch = read_pipe[idx];
                            message += ch;
                            idx++;
                        }

                        //******************************//
                        //    Write to Children Pipes   // 
                        //******************************//
                        // <ACTION><PID><MESSAGE_LENGTH><MESSAGE>
                        char action = 'R';
                        std::string temp = action+destination+" "+message_len+" "+message; 
                        char out_message[BUFF_SIZE];
                        strcpy(out_message, temp.c_str());
                        // cout << "[DEBUG] PARENT" << " is sending child_" << destination <<" "<< out_message <<endl;
                        write(fd_ptc[stoi(destination)][1], &out_message, BUFF_SIZE);
                    }
                }
                if(terminate_IPC == 0) {
                    // send terminator value to all children 
                    char end = 'T';
                    for(int k = 0; k < child_count; k++){
                        write(fd_ptc[k][1], &end, BUFF_SIZE);
                        // close parent to child pipe [write] 
                        close(fd_ptc[k][1]);
                    }
                }
            }
      
        }
    }

    //-------------------------------------------------//
    //          Wait for Children Gen 1                //
    //-------------------------------------------------//
    /* The parent/dispatcher process waits for all child 
    processes to terminate,then writes an output file 
    in the scrap folder as well, then terminates. */ 
    int parent;
    while((parent=wait(NULL))>0);



    // cout << "\nonly print 1 time\n " << endl;
    if(pid != 0){
        // cout << "All Children Processes Are Terminated" << endl;
        // get file paths of 1st gen children output 
        std::vector<std::string> child_output_paths;
        file_paths(scrap_path, &child_output_paths);
        //-------------------------------------------------//
        //            Create Children Gen 2                //
        //-------------------------------------------------//
        // cout << "[DEBUG] the Child Count is " << child_count << endl;
        for(int i = 0; i < child_count; i++){
            if(pid != 0){
                pid = fork();
                if(pid ==0){
                    /* Child i opens its assigned folder from scrap,
                    it reads the file path listed, opens the file, 
                    reads the contents of the file, and then writes 
                    to a new file the content. The process will loop 
                    though all of the file paths and append its contents 
                    to the new file. */ 
                    // create new output file
                    // cout << "[DEBUG] The Scrap path for Gen2 Children are: " << child_output_paths[i] << endl;
                    second_child_output(scrap_path, child_output_paths[i],i);
                    return 0;
                }else{
                    // parent does work 
                }   
            }
        }
    }
    return 0;
}