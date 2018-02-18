/* 
 * File:   consumer.c
 * Author: Michael Beckering
 *
 * Created on February 13, 2018, 10:10 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHMKEY_BUFFLAGS 0425000
#define SHMKEY_TURN 04251
#define SHMKEY_FLAGARR 042522
#define SHMKEY_BUF1 04253
#define SHMKEY_BUF2 04254
#define SHMKEY_BUF3 04255
#define SHMKEY_BUF4 0425666
#define SHMKEY_BUF5 04257
#define BUFF_SZ sizeof (int)

/*
 * 
 */
int main(int argc, char** argv) {
    int randomTime;
    int me = atoi(argv[1]);
    int c_total = atoi(argv[2]);
    int i;
    
    printf("consumer %d launched\n", me);
    
    //access shared memory for turn variable (int)
    int shmid_turn = shmget(SHMKEY_TURN, BUFF_SZ, 0777);
        if (shmid_turn == -1) { //terminate if shmget failed
            perror("Error in consumer shmget turn");
            return 1;
        }
    //get pointer to shared turn variable (int)
    int *turn = (int*) shmat(shmid_turn, 0, 0);
    
    //access to shared memory for flag array
    int shmid_flagarr = shmget(SHMKEY_FLAGARR, c_total*BUFF_SZ, 0777);
    if (shmid_flagarr == -1) { //terminate if shmget failed
        perror("Error in shmget");
        return 1;
    }
    //get pointer to the shared flag array
    int* flag;
    flag = shmat(shmid_flagarr, 0, 0);
    
    //printf("flag[0]=%d\n", flag[0]);
    for (i=0; i<5; i++) {
        sleep(1);
    }
    
    //multiprocess solution to mutual exclusion taken from lecture notes
    //me is this consumer's number
    enum state {idle, want_in, in_cs};
    int j; //it's this consumer's turn
    
    /*
    do {
        do {
            flag[me] = want_in; //raise my flag
            j = *turn; //set local variable
            //now wait until it's my turn
            while (j != me)
                j = (flag[j] != idle) ? *turn : (j +1) % c_total;
            
            //declare intention to enter party zone
            flag[me] = in_cs;
            
            //make sure nobody else is in party zone (me only party alone)
            for ( j=0; j<c_total; j++ )
                if ( ( j != me ) && ( flag[j] == in_cs ) )
                    break;
        } while ( ( j<c_total) || ( *turn != me && flag[*turn] != idle ) );
        
        //assign turn to self and enter party zone
        *turn = me;
        //***CRITICAL SECTION HERE***
        
        //exit section
        j = (*turn + 1) % c_total;
        while (flag[j] == idle)
            j = (j + 1) % c_total;
        
        //assign turn to next waiting process & change my flag to idle
        *turn = j;
        flag[me] = idle;
        
        //remainder section: sleepy time
        randomTime = rand() %5 + 1;
        printf("consumer % d sleeping %d sec...\n", me, randomTime);
        sleep(randomTime);
    } while (1);
    */
    printf("consumer %d terminated: normal. pid: %ld\n", me, getpid());
    return 1;
}

