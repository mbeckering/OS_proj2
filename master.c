/* 
 * File:   master.c
 * Author: Michael Beckering
 * Created: Monday, Feb 12, 2018
 * CS4760-E01 Spring 2018 Project 2
 * Created on February 12, 2018, 10:19 AM
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define SHMKEY_TURN 04251
#define SHMKEY_FLAGARR 04252
#define BUFF_SZ sizeof (int)
#define BILLION 1000000000L

void sigintHandler(int);
static int setperiodic(double);

/*
 * 
 */
int main(int argc, char** argv) {
    //initialize variables
    char * turn_paddr; //address pointer for turn variable
    char * flagarr_paddr; //address pointer for flag array
    int option; //command line option holder for getopt
    int shmid_turn, shmid_flagarr; //shmid holders
    int hflag = 0; //getopt help flag
    int nflag = 0; //getopt number of processes flag
    int status = 0; //status holder for children processes
    int n, i, wstatus; //num processes, iterator, wpid status holder
    int proc_limit = 18; //20-2: accounting for bash and master
    int proc_count = 0; //currently running child process count
    pid_t producerpid, consumerpid, wpid; //pid holders
    char consumer_num[10]; //string for holding arg1 for exec calls
    char total_procs[10]; //string for holding arg2 for exec calls
    
    signal(SIGINT, sigintHandler);
    
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
    
    //if only -n is selected
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
        
        //THE GOOD STUFF********************************************************
        
        //create and attach to shared memory for turn variable before forking
        shmid_turn = shmget(SHMKEY_TURN, BUFF_SZ, 0777 | IPC_CREAT);
        printf("shmid_turn = %d\n", shmid_turn);
        if (shmid_turn == -1) { //terminate if shmget failed
            perror("Error in shmget");
            return 1;
        }
        //SHARED TURN TESTING
        int *turn = (int*) shmat(shmid_turn, 0, 0);
        *turn = 0; //WRITING INTO SHARED AREA
        //create and attach to shared memory for flag array before forking
        shmid_flagarr = shmget(SHMKEY_FLAGARR, n*BUFF_SZ, 0777 | IPC_CREAT);
        printf("shmid_flagarr = %d\n", shmid_flagarr);
        if (shmid_flagarr == -1) { //terminate if shmget failed
            perror("Error in shmget");
            return 1;
        }
        
        //fork the producer process
        if ( (producerpid = fork()) <0 ){ //terminate code
            perror("error forking producer");
            return 1;
        }
        if ( producerpid == 0 ) { //producer process child code
            execl("./producer", NULL);
            perror("execl() failure on producer");
            return 1;
        }
        
        printf("master: beginning fork loop\n");
        //fork consumer(s), max proc_limit (18) at a time
        for (i=0; i<n; i++) {
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
                sprintf(consumer_num, "%d", i+1); //build arg1 string
                sprintf(total_procs, "%d", n); //build arg2 string
                execlp("./consumer", "./consumer", consumer_num, total_procs, (char *)NULL);
                perror("execl() failure on consumer"); //report & exit if exec fails
                return 1;
            }
            //parent code proceeds, increment proc_count, loop back
            proc_count++;
        }
        
        //END GOOD STUFF********************************************************
    }
    //if no options are selected from command line, print usage and exit
    //NOTE: Maybe add this condition to run default of 10 processes instead?
    else {
        printf("Usage: %s -n <# of processes>\n", argv[0]);
        return 0;
    }
    
    //wait for all children to finish
    while ( (wpid = wait(&status)) > 0);
    
    //remove shared memory for turn variable or report via perror and exit
    if ( shmctl(shmid_turn, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
        return 1;
    }
    
    //remove shared memory for flag array or report perror and exit
    if ( shmctl(shmid_flagarr, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
        return 1;
    }
    
    //if this point is reached, normal shutdown is achieved
    printf("%s: shutting down: normal.\n", argv[0]);
    return 0;
}

//basis for this function is from geeksforgeeks.org
void sigintHandler(int sig_num) {
    int sh_status;
    pid_t sh_wpid;
    signal(SIGINT, sigintHandler);
    printf("master: ctrl+c CAUGHT.\n");
    printf("master: killing all children...\n");
    //kill children here
    //wait for all children to finish
    printf("master: waiting for all children to finish...\n");
    while ( (sh_wpid = wait(&sh_status)) > 0);
    
}

static int setperiodic(double sec) {
    timer_t timerid;
    struct itimerspec value;
    
    if (timer_create(CLOCK_REALTIME, NULL, &timerid) == -1)
        return -1;
    value.it_interval.tv_sec = (long)sec;
    value.it_interval.tv_nsec = (sec - value.it_interval.tv_sec)*BILLION;
    if (value.it_interval.tv_nsec >= BILLION) {
        value.it_interval.tv_sec++;
        value.it_interval.tv_nsec -= BILLION;
    }
    value.it_value = value.it_interval;
    return timer_settime(timerid, 0, &value, NULL);
}