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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHMKEY 859047
#define BUFF_SZ sizeof (int)

/*
 * 
 */
int main(int argc, char** argv) {
    
    //initialize variables
    char * paddr;
    int option;
    int shmid;
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
        
        //create and attach to shared memory before forking
        shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
        printf("shmid = %d\n", shmid);
        if (shmid == -1) { //terminate if shmget failed
            perror("Error in shmget");
            return 1;
        }
                
        //get pointer to the shared block ** NECESSARY in master??
        paddr = (char*)(shmat (shmid, 0, 0) );
        int * pint = (int*) (paddr);
        *pint = 420;
        printf("pint = %d\n", (int)*pint);
        
        //fork producer
        if ( (producerpid = fork()) <0 ){ //terminate code
            perror("error forking producer");
            return 1;
        }
        if ( producerpid == 0 ) { //producer child code
            execl("./producer", NULL);
            perror("execl() failure on producer");
            return 1;
        }
        
        //fork consumer(s), max 18 at a time
        for (i=0; i<n; i++) {
            //printf("i=%d, proc_count=%d\n", i, proc_count);
            while (proc_count == proc_limit) { //if max # of processes is reached...
                if ( waitpid(-1, &wstatus, WNOHANG) < 0 ) { //...wait for any child to finish
                    proc_count--; //then decrement process count, report and proceed
                    printf("%s: child termination detected, proc_count "
                            "decremented to %d\n", argv[0], proc_count);
                }
            }
            if ( (consumerpid = fork()) < 0 ){ //terminate code
                perror("Error forking child");
                return 1;
            }
            if (consumerpid == 0) { //child code
                char arg[10];
                sprintf(arg, "%d", i+1);
                execlp("./consumer", "./consumer", arg, (char *)NULL);
                perror("execl() failure on consumer");
                return 1;
            }
            //parent code proceeds
            proc_count++;
            printf("proc_count incremented to %d\n", proc_count);
            
        }
        
        //END MAIN STUFF_*******************************************************
    }
    //if no options are selected, print usage and exit
    else {
        printf("Usage: %s -n <# of processes>\n", argv[0]);
        return 0;
    }
    
    while ( (wpid = wait(&status)) > 0); //wait for all children to finish
    
    //detach shared memory
    if ( shmdt(paddr) == -1) {
        perror("shmdt error");
        return 1;
    }
    
    printf("%s: shutting down: normal.\n", argv[0]);
    return 0;
}


