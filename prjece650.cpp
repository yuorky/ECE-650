#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <signal.h>
#include "graph.h"
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

// Define a semaphore as SAT function timeout flag
static sem_t to_flag;

// Define a struct to store thread data and the return value of subthreads
struct thread_data{
    int vertex_num;
    float time;
    std::vector<std::vector<int>> vertex;
    std::vector<int> cover;
};

// Define a function to parse the input start with "V"
// Return the number of vertex
int parseV(std::vector<std::string> command){
    if (command.size()==2){
        int v_n = atoi(command[1].c_str()); // c_str() convert string to const char* for atoi()
        if (v_n>0){
            return v_n;
        }else{
            std::cerr << "Error: V input error.\n";
            return 0;
        }
    }else{
        std::cerr << "Error: V input error.\n";
        return 0;
    }
}

// Define a function to parse the input start with "E"
// Return a 2D vector of vertices pairs in edges
std::vector<std::vector<int>> parseE(std::vector<std::string> command){
    std::vector<std::vector<int>> vertex;
    if (command.size() == 2){
        std::string edge = command[1]; // store in string first

        // using try-catch to catch the exception when parsing the edges to vertices
        try {
            for (unsigned i=0; i<edge.length(); i++){
                if (edge.at(i)=='<'){
                    int n1, n2;
                    unsigned j = i+1;
                    while (edge.at(j) != ',') j++;
                    n1 = atoi(edge.substr(i+1,j++).c_str());
                    i = j;
                    while (edge.at(j) != '>') j++;
                    n2 = atoi(edge.substr(i,j).c_str());
                    i = j;
                    std::vector<int> pair(2);
                    pair[0] = n1;
                    pair[1] = n2;
                    vertex.push_back(pair);
                }
            }
        } catch (std::exception& e) {
            std::cerr << "Error: Parsing E input error:" << e.what() << "\n"; //.what() return the reason of exception
        }
    }else{
        std::cerr << "Error: E Input error!\n";
    }
    return vertex;
}

// This function is used to generate real CPU time from a clockid
int Pclock(clockid_t cid){
    struct timespec ts;
    if (clock_gettime(cid, &ts) == -1){
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    int time = ts.tv_sec*1000000 + ts.tv_nsec/1000; // convert to microsecond
    return time;
}

// Signal handler for timeout
void TimeOut(int n){
    sem_post(&to_flag); // unlock the semaphore by increasing to_flag
}

// CNF-SAT subthread function
void *CNF_SAT_VC(void *arg){
    struct thread_data *data;
    // Decode thread data
    data = (struct thread_data *) arg;
    auto graph = new Graph<int>;
    // Initialize the graph object
    graph->vert_num = data->vertex_num;
    graph->Generate(data->vertex);
    // Invoke the CNF_SAT_VC function
    data->cover = graph->CNF_SAT_VC();
    // Set the timeout flag and inform the main thread that the function is done
    sem_post(&to_flag); // unlock the semaphore
    graph->~Graph();
    // Print the result
    std::cout << "CNF_SAT_VC: ";
    for(int i=0; i < data->cover.size(); i++){
        if (i==data->cover.size()-1){
            std::cout << data->cover[i] << "\n";
        }else{
            std::cout << data->cover[i] << ",";
        }
    }
    // Compute the running time of the function
    clockid_t cid;
    int retcode;
    retcode = pthread_getcpuclockid(pthread_self(), &cid);
    if (retcode){
        perror("pthread_getcpuclockid");
    }else{
        data->time = Pclock(cid);
    }
}

// APPROX_VC_1 subthread function
void *APPROX_VC_1(void *arg){
    struct thread_data *data;
    // Decode thread data
    data = (struct thread_data *) arg;
    auto graph = new Graph<int>;
    // Initialize the graph object
    graph->vert_num = data->vertex_num;
    graph->Generate(data->vertex);
    // Invoke the APPROX_VC_1 function
    data->cover = graph->APPROX_VC_1();
    graph->~Graph();
    // Print the result
    std::cout << "APPROX_VC_1: ";
    for(int i=0; i<data->cover.size(); i++){
        if(i == data->cover.size()-1){
            std::cout << data->cover[i] << "\n";
        }else{
            std::cout << data->cover[i] << ",";
        }
    }
    // Compute the running time of the function
    clockid_t cid;
    int retcode;
    retcode = pthread_getcpuclockid(pthread_self(), &cid);
    if (retcode){
        perror("pthread_getcpuclockid");
    }
    else{
        data->time = Pclock(cid);
    }
}

// APPROX_VC_2 subthread function
void *APPROX_VC_2(void *arg){
    struct thread_data *data;
    // Decode the data
    data = (struct thread_data *) arg;
    auto graph = new Graph<int>;
    // Initialize the graph object
    graph->vert_num = data->vertex_num;
    graph->Generate(data->vertex);
    // invoke the APPROX_VC_2 function
    data->cover = graph->APPROX_VC_2();
    graph->~Graph();
    // print the result
    std::cout << "APPROX-VC-2: ";
    for(int i=0; i<data->cover.size(); i++){
        if (i == data->cover.size()-1){
            std::cout << data->cover[i] << "\n";
        }else{
            std::cout << data->cover[i] << ",";
        }
    }
    // Compute the running time of the function
    clockid_t cid;
    int retcode;
    retcode = pthread_getcpuclockid(pthread_self(), &cid);
    if (retcode){
        perror("pthread_getcpuclockid");
    }
    else{
        data->time = Pclock(cid);
    }
}

int main(int argc, char **argv) {
    std::ofstream f_out("output.csv", std::ios::app);
    f_out << "V" << "," << "Time(SAT)" << "," << "Time(A1)" << "," << "Time(A2)" << "," << "Approx1" << "Approx2" << std::endl;
    f_out.close();

    // Continue to get input until seeing "eof".
    while (!std::cin.eof()){
        std::string line;
        getline(std::cin, line);
        std::istringstream input(line);
        std::vector<std::string> command;

        // Read the input string by stream.
        while(!input.eof()){
            std::string s;
            input >> s;
            if (input.fail()){
                break;
            }
            else {
                command.push_back(s);
            }
            if (input.eof()){
                break;
            }
        }
        // Define 3 thread data structs to store data from the input
        struct thread_data td1, td2, td3;

        // Parse inputs
        if (command.empty()){
            continue;
        }
        if (command[0] == "V"){
            // Store the number of vertices
            td1.vertex_num = parseV(command);
            td2.vertex_num = parseV(command);
            td3.vertex_num = parseV(command);
            continue;
        }
        if (command[0] == "E"){
            std::ofstream f_out("output.csv", std::ios::app); //all output are performed at the end of the file
            f_out << td1.vertex_num << ",";
            // Store the coordinates of vertices
            td1.vertex = parseE(command);
            td2.vertex = parseE(command);
            td3.vertex = parseE(command);

            // Initialize the timeout semaphore
            sem_init(&to_flag, 0, -1);
            int retcode;
            // Register the handler function to SIGALRM (termination request)
            signal(SIGALRM, TimeOut); // signal (registered signal, signal handler (function))

            // Initialize itimerval struct
            struct itimerval tick;
            memset(&tick, 0, sizeof(tick)); // so the tick is all zero
            // Set the timeout value
            tick.it_value.tv_sec = 20; // wait for 20s, 0 microsecond
            tick.it_value.tv_usec = 0;

            // Start the timer
            retcode = setitimer(ITIMER_REAL, &tick, NULL);

            // If setting timer failed print error message
            if (retcode){
                perror("setitimer");
            }

            // Start 3 subthreads to run 3 functions
            pthread_t t1, t2, t3;
            // pthread_create accepts 4 args:
            // 1. pointer of the thread ID, it will update the value in it
            // 2. attributes to set the properties of thread
            // 3. Function pointer to function that thread will run in parallel on start
            // 4. arguments to be passed to function
            retcode = pthread_create(&t1, NULL, CNF_SAT_VC, (void *)&td1);
            // pthread_create will return 0 if successes.
            if(retcode != 0){
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
            retcode = pthread_create(&t2, NULL, APPROX_VC_1, (void *)&td2);
            if(retcode != 0){
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
            retcode = pthread_create(&t3, NULL, APPROX_VC_2, (void *)&td3);
            if(retcode != 0){
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
            // wait for timeout or the complete of t1
            sem_wait(&to_flag);
            // If t1 times out then cancel the thread
            if(td1.cover.size() == 0){
                solver->interrupt();
                // send a cancellation request to a thread
                retcode =  pthread_cancel(t1);
                if(retcode){
                    perror("pthread_cancel");
                }
                // detach a thread
                retcode = pthread_detach(t1);
                if(retcode){
                    perror(("pthread_detach"));
                }
                std::cout << "CNF_SAT_VC: Time out\n";
                retcode = pthread_join(t2, NULL);
                if(retcode){
                    perror("pthread_join");
                }
                retcode = pthread_join(t3, NULL);
                if(retcode){
                    perror("pthread_join");
                }
                f_out << "TO" << "," << td2.time << "," << td3.time << "," << "N/A" << "," << "N/A" << std::endl;
            }
            // If t1 completes in time then cancel the timer and output the approximation ratio information
            else{
                tick.it_value.tv_sec = 0;
                tick.it_value.tv_usec = 0;
                retcode = setitimer(ITIMER_REAL, &tick, NULL);
                if(retcode){
                    perror("setitimer");
                }
                retcode = pthread_join(t1, NULL);
                if(retcode){
                    perror("pthread_join");
                }
                retcode = pthread_join(t2, NULL);
                if(retcode){
                    perror("pthread_join");
                }
                retcode = pthread_join(t3, NULL);
                if(retcode){
                    perror("pthread_join");
                }
                float ratio1 = (float)td2.cover.size()/(float)td1.cover.size();
                float ratio2 = (float)td3.cover.size()/(float)td1.cover.size();
                f_out << td1.time << "," << td2.time << "," << td3.time << "," << ratio1 << "," << ratio2 << std::endl;
            }
            f_out.close();
            continue;
        }
        std::cerr << "Error: Input error!\n";
    }
    return 0;
}
