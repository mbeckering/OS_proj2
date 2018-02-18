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

#define SHMKEY_BUFFLAGS 0425000
#define SHMKEY_TURN 04251
#define SHMKEY_FLAGARR 042522
#define SHMKEY_BUF1 04253
#define SHMKEY_BUF2 04254
#define SHMKEY_BUF3 04255
#define SHMKEY_BUF4 0425666
#define SHMKEY_BUF5 04257
#define BUFF_SZ sizeof (int)
#define BILLION 1000000000L

static int setperiodic(double);
static int setinterrupt();
static void interrupt(int signo, siginfo_t *info, void *context);
void clearShm();
void siginthandler(int sig_num);

pid_t childpids[25];
pid_t producerpid;
int n;
int shmid_turn, shmid_flagarr, shmid_buf1, shmid_buf2,
            shmid_buf3, shmid_buf4, shmid_buf5, shmid_bufflags; //shmid holders
/*
 * 
 */
int main(int argc, char** argv) {
    //initialize variables
    char * turn_paddr; //address pointer for turn variable
    char * flagarr_paddr; //address pointer for flag array
    int option; //command line option holder for getopt
    int hflag = 0; //getopt help flag
    int nflag = 0; //getopt number of processes flag
    int status = 0; //status holder for children processes
    int i, wstatus; //num processes, iterator, wpid status holder
    int proc_limit = 18; //20-2: accounting for bash and master
    int proc_count = 0; //currently running child process count
    pid_t consumerpid, wpid; //pid holders
    char consumer_num[10]; //string for holding arg1 for exec calls
    char total_procs[10]; //string for holding arg2 for exec calls
    double MAXTIME = 30.0; //max timeout for master
    
    // Set up interrupt handler
    signal (SIGINT, siginthandler);
    if (setinterrupt() == -1) {
        perror("Failed to set up SIGALRM handler");
        return 1;
    }
    // Set up periodic timer
    if (setperiodic(MAXTIME) == -1) {
        perror("Failed to setup periodic interrupt");
        return 1;
    }
    
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
        printf("Usage: %s -n <# of processes: MAX 18>\n", argv[0]);
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
        else if (( n = atoi(argv[2])) >= 1 && n <= 18) {
            printf("%s: Proceeding with %d consumer processes\n",
                    argv[0], n);
        }
        //otherwise exit with error message
        else {
            printf("%s: Error: option -n range = {1-18}\n", argv[0]);
            return 1;
        }
        
        //THE GOOD STUFF********************************************************
        
        //create and attach to shared memory for turn variable before forking
        shmid_turn = shmget(SHMKEY_TURN, BUFF_SZ, 0777 | IPC_CREAT);
        if (shmid_turn == -1) { //terminate if shmget failed
            perror("Error in shmget");
            return 1;
        }
        //SHARED TURN TESTING
        int *turn = (int*) shmat(shmid_turn, 0, 0);
        *turn = 0; //WRITING INTO SHARED AREA
        //create and attach to shared memory for flag array before forking
        shmid_flagarr = shmget(SHMKEY_FLAGARR, n*BUFF_SZ, 0777 | IPC_CREAT);
        if (shmid_flagarr == -1) { //terminate if shmget failed
            perror("Error in shmget");
            return 1;
        }
        //set up shared memory for bufflags
        shmid_bufflags = shmget(SHMKEY_BUFFLAGS, 5*BUFF_SZ, 0777 | IPC_CREAT);
        if (shmid_bufflags == -1) { //terminate if shmget failed
            perror("Error in shmget for bufflags");
            return 1;
        }
        
        //create and attach to shared memory for buffers

        shmid_buf1 = shmget(SHMKEY_BUF1, 100, 0777 | IPC_CREAT);
        if (shmid_buf1 == -1) { //terminate if shmget failed
            perror("Error in shmget for buf1");
            return 1;
        }
        shmid_buf2 = shmget(SHMKEY_BUF2, 100, 0777 | IPC_CREAT);
        if (shmid_buf2 == -1) { //terminate if shmget failed
            perror("Error in shmget for buf2");
            return 1;
        }
        shmid_buf3 = shmget(SHMKEY_BUF3, 100, 0777 | IPC_CREAT);
        if (shmid_buf3 == -1) { //terminate if shmget failed
            perror("Error in shmget for buf3");
            return 1;
        }
        shmid_buf4 = shmget(SHMKEY_BUF4, 100, 0777 | IPC_CREAT);
        if (shmid_buf4 == -1) { //terminate if shmget failed
            perror("Error in shmget for buf4");
            return 1;
        }
        shmid_buf5 = shmget(SHMKEY_BUF5, 100, 0777 | IPC_CREAT);
        if (shmid_buf5 == -1) { //terminate if shmget failed
            perror("Error in shmget for buf5");
            return 1;
        }
        
        //fork the producer process
        if ( ( producerpid = fork()) <0 ){ //terminate code
            perror("error forking producer");
            return 1;
        }
        if ( producerpid == 0 ) { //producer process child code
            execl("./producer", NULL);
            perror("execl() failure on producer");
            return 1;
        }
        
        //fork consumer(s), max proc_limit (18) at a time
        for (i=0; i<n; i++) {
            if ( (consumerpid = fork()) < 0 ){ //terminate code
                perror("Error forking consumer");
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
            proc_count++; //unneeded?
            childpids[i] = consumerpid; //store consumer pids
        }
        
        //END GOOD STUFF********************************************************
    }
    //if no options are selected from command line, print usage and exit
    //NOTE: Maybe add this condition to run default of 10 processes instead?
    else {
        printf("Usage: %s -n <# of processes: MAX 18>\n", argv[0]);
        return 0;
    }
    
    //wait for all children to finish
    while ( (wpid = wait(&status)) > 0);
    
    //clear shared memory
    printf("master: Clearing shared memory...\n");
    clearShm();
    
    //if this point is reached, normal shutdown is achieved
    printf("%s: shutting down\n", argv[0]);
    return 0;
}

void clearShm() {
    //remove shared memory for turn variable or report via perror and exit
    if ( shmctl(shmid_turn, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    //remove shared memory for flag array or report perror and exit
    if ( shmctl(shmid_flagarr, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    if ( shmctl(shmid_bufflags, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    //remove shared memory for buffers
    if ( shmctl(shmid_buf1, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    if ( shmctl(shmid_buf2, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    if ( shmctl(shmid_buf3, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    if ( shmctl(shmid_buf4, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
    if ( shmctl(shmid_buf5, IPC_RMID, NULL) == -1) {
        perror("error removing shared memory");
    }
}

//this function taken from UNIX text
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

//this function taken from UNIX text
static int setinterrupt() {
    struct sigaction act;
    
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = interrupt;
    if ((sigemptyset(&act.sa_mask) == -1) ||
            (sigaction(SIGALRM, &act, NULL) == -1))
        return -1;
    return 0;
}

static void interrupt(int signo, siginfo_t *info, void *context) {
    int sh_status, i;
    pid_t sh_wpid;
    printf("master: Timer Interrupt Detected! signo = %d\n", signo);
    printf("master: Killing children...\n");
    kill(producerpid, SIGINT);
    for (i=0; i<n; i++) {
        kill(childpids[i], SIGINT);
        printf("killing %ld\n", childpids[i]);
    }
    //clearShm();
}

void siginthandler(int sig_num) {
    int sh_status, i;
    pid_t sh_wpid;
    printf("master: Ctrl+C interrupt detected! signo = %d\n", sig_num);
    printf("master: Killing children...\n");
    kill(producerpid, SIGINT);
    for (i=0; i<n; i++) {
        kill(childpids[i], SIGINT);
        printf("killing %ld\n", childpids[i]);
    }
    //wait for all children to finish
    while ( (sh_wpid = wait(&sh_status)) > 0);
    //clearShm();
}