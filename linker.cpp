#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char**argv){
    std::vector<pid_t> kids;

    // create a pipe
    int rgentoa1[2], a1a3toa2[2];
    pipe(rgentoa1);
    pipe(a1a3toa2);

    pid_t rgen_pid, a1_pid, a2_pid;
    rgen_pid = fork();

    if (rgen_pid==0){ // child process of rgen

        // redirects rgen stdout to the pipe, exec rgen
        dup2(rgentoa1[1], STDOUT_FILENO);
        close(rgentoa1[0]);
        close(rgentoa1[1]);

        close(a1a3toa2[0]);
        close(a1a3toa2[1]); // close this too

        // int execv(const char *file, char *const argv[ ])
        // nullptr: The argument list must end with a NULL.
        char *arg_1[] = {(char *)"./rgen", reinterpret_cast<char *>(argv), nullptr};
        execv(arg_1[0],arg_1);
        if (execv(arg_1[0],arg_1)==-1){
            cerr << strerror(int errno) << "\n";
        }
    }

    kids.push_back(rgen_pid);

    a1_pid=fork();
    if (a1_pid == 0){
        // redirect stdin from the pipe to a1
        // redirect a1 stdout to the pipe
        // exec a1ece650.py
        dup2(rgentoa1[0], STDIN_FILENO);
        dup2(a1a3toa2[1], STDOUT_FILENO);
        close(rgentoa1[0]);
        close(rgentoa1[1]);
        close(a1a3toa2[0]);
        close(a1a3toa2[1]);

        char *arg_2[] = {(char *)"./a1ece650.py", nullptr}; // there is no keyboard arguments

        execv(arg_2[0],arg_2);
//        if (execvp("python",arg_2)==-1){
//            cerr << strerror(int errno) << "\n";
//            abort();
//        }
    }

    kids.push_back(a1_pid);

    a2_pid=fork();
    // redirect stdin from pipe to a2
    if (a2_pid==0){
        dup2(a1a3toa2[0],STDIN_FILENO);
        close(rgentoa1[0]);
        close(rgentoa1[1]);
        close(a1a3toa2[0]);
        close(a1a3toa2[1]);

        char *arg_3[] = {(char *)"./a2ece650", nullptr};
        execv(arg_3[0],arg_3);
        if (execv(arg_3[0], arg_3) == -1){
            cerr << strerror(int errno) << "\n";
            abort();
        }
    }

    kids.push_back(a2_pid);

    // finally, redirect a2 stdout to the pipe
    dup2(a1a3toa2[1], STDOUT_FILENO);
    close(rgentoa1[0]);
    close(rgentoa1[1]);
    close(a1a3toa2[0]);
    close(a1a3toa2[1]);

    while(!cin.eof()){
        string input;
        getline(cin,input);
        cout << input << endl;
    }

    // send kill signal to all children
    for (pid_t k: kids){
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return 0;
}

