/* 
 * File:   master.c
 * Author: Michael Beckering
 * Created: Monday, Feb 12, 2018
 * CS4760-E01 Spring 2018 Project 2
 * Created on February 12, 2018, 10:19 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * 
 */
int main(int argc, char** argv) {
    
    //initialize variables
    int option;
    int hflag = 0;
    int nflag = 0;
    int status = 0; //status holder for children processes
    int n, i, wstatus;
    int proc_limit = 18; //20-2: accounting for bash and master
    int proc_count = 0;
    pid_t producerpid, consumerpid, wpid;
    
    //getopt loop to parse command line options
    while ((option = getopt(argc, argv, "hn")) != -1) {
        switch(option) {
            case 'h':
                hflag = 1;     
                break;
            case 'n':
                nflag = 1;
                break;
            default:
                nflag = 1;
                hflag = 1;
                break;
        }
    }
    
    //if only -h is selected, print usage
    if ( (hflag == 1) && (nflag ==0) ) {
        printf("Usage: %s -n <# of processes>\n", argv[0]);
        return 0;
    }
    
    else if ( (nflag == 1) && (hflag == 0) ) {
        //if no argument, use default of 10 processes
        if ( argv[2] == NULL ) {
            n = 10;
            printf("%s: Proceeding with default of %d consumer processes\n",
                    argv[0], n);
        }
        //continue with argument if it's positive
        else if (( n = atoi(argv[2])) >= 1) {
            printf("%s: Proceeding with %d consumer processes\n",
                    argv[0], n);
        }
        //otherwise exit with error message
        else {
            printf("%s: Error: Non-zero argument required for option -n\n", argv[0]);
            return 1;
        }
        
        //MAIN STUFF HERE*******************************************************
        
        //fork producer
        if ( (producerpid = fork()) <=0 ){ //child code
            execl("./producer", NULL);
            return 1;
        }
        
        //fork consumer(s), max 18 at a time
        
        for (i=0; i<n; i++) {
            while (proc_count == proc_limit) {
                if ( waitpid(-1, &wstatus, WNOHANG) < 0 ) {
                    proc_count--;
                    printf("%s: child termination detected, proc_count decremented to %d\n", argv[0], proc_count);
                }
            }
            if ( (consumerpid = fork()) <=0 ){ //child code
                execl("./consumer", NULL);
                printf("%s: consumer %d shutting down.\n", argv[0], n);
                return 1;
            }
            proc_count++;
        }
        
        //END MAIN STUFF_*******************************************************
    }
    //if no options are selected, print usage and exit
    else {
        printf("Usage: %s -n <# of processes>\n", argv[0]);
        return 0;
    }
    
    while ( (wpid = wait(&status)) > 0); //wait for all children to finish
    printf("%s: shutting down: normal.\n", argv[0]);

    return 1;
}


