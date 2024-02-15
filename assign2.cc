/********************************************************************
* CSCI 480 - Assignment 1 - Spring 2024
* Programmer: Christopher Garland
* Section: 1
* TA: Ajay Kuma Reddy Kandula
* Date Due: 2/16/24
* Purpose: To practice using system calls such as fork() and pipe().
*********************************************************************/

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sstream>

using namespace std;

#define BUFFER_SIZE 25
#define REND 0
#define WEND 1

int main() {
    char write_msg[BUFFER_SIZE] = "Go do some chores.";
    char read_msg[BUFFER_SIZE];
    pid_t child_1;
    pid_t child_2; 
    int afd[2];
    int bfd[2];

    /**
     * create pipe a
     * (initial process <-> grandchild)
    */
    if (pipe(afd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    /**
     * create pipe b
     * (intermediate parent <-> grandchild)
    */
    if (pipe(bfd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    /**
     * first call to fork() creates one additional process
    */
    child_1 = fork();
    if (child_1 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    /**
     * second call to fork() creates 2 additional processes
     * for a total of 3 additional processes
    */
    child_2 = fork();
    if (child_2 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (child_1 > 0 && child_2 > 0) {
        /* parent process */
        setbuf(stdout, NULL);

        /* close unused end */
        close(afd[REND]);
        /* write to pipe a */
        write(afd[WEND], write_msg, strlen(write_msg)+1);
        /* close write end */
        close(afd[WEND]);

        printf("\nParent: My PID is %d, my parent's PID is %d, I write to pipe A: %s\n", getpid(), getppid(), write_msg);

        /* build command */
        stringstream ss("");
        ss << "/bin/ps -f --ppid " << getppid() << "," << getpid() << "," << child_1 << "," << child_2;
        string cmd = ss.str();

        printf("\nParent: Issuing command: %s\n", cmd.c_str());

        /* list all proceses involved */
        if (system(cmd.c_str()) < 0) { perror("system"); exit(EXIT_FAILURE); }

        /* wait for all children to finish */
        for (int i = 0; i < 4; i++) wait(NULL);

        cout << "\nParent: Child processes finished their work.\n";
        exit(EXIT_SUCCESS);

    } else if (child_1 == 0 && child_2 > 0) {
        /* intermediate parent */

        /* close unused end */
        close(bfd[REND]);
        /* write pipe b */
        write(bfd[WEND], write_msg, strlen(write_msg)+1);
        /* close write end */
        close(bfd[WEND]);

        printf("\nIntermediate Parent: My PID is %d, my parent's PID is %d, I write to pipe B: %s\n", getpid(), getppid(), write_msg);
        
        sleep(3);

        cout << "\nIntermediate Parent " << getpid() << ": I am awake.\n";
        setbuf(stdout, NULL);
        exit(EXIT_SUCCESS);

    } else if (child_1 > 0 && child_2 == 0) {
        /* (non-parent) child of initial process */

        /* close unused end */
        close(afd[WEND]);
        /* read from pipe a */
        read(afd[REND], read_msg, BUFFER_SIZE);
        /* close read end */
        close(afd[REND]);

        printf("\nChild: My PID is %d, my parent's PID is %d, I read from pipe A: %s\n", getpid(), getppid(), read_msg);

        sleep(3);

        cout << "\nChild " << getpid() << ": I am awake.\n";
        setbuf(stdout, NULL);
        exit(EXIT_SUCCESS);

    } else { // if child_1 = 0 && child_2 == 0
        /* grandchild */

        /* close unused end */
        close(bfd[WEND]);
        /* read from pipe b */
        read(bfd[REND], read_msg, BUFFER_SIZE);
        /*close read end */
        close(bfd[REND]);

        printf("\nChild: My PID is %d, my parent's PID is %d, I read from pipe B: %s\n", getpid(), getppid(), read_msg);

        sleep(3);

        cout << "\nChild " << getpid() << ": I am awake.\n";
        setbuf(stdout, NULL);
        exit(EXIT_SUCCESS);
    }
   
}