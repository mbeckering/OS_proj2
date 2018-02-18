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
#define SHMKEY_buf0 04253
#define SHMKEY_buf1 04254
#define SHMKEY_buf2 04255
#define SHMKEY_buf3 0425666
#define SHMKEY_buf4 04257
#define BUFF_SZ sizeof (int)

/*
 * 
 */
int main(int argc, char** argv) {
    int randomTime;
    int me = atoi(argv[1]) - 1;
    int c_total = atoi(argv[2]);
    int i;
    int shmid_buf0, shmid_buf1, shmid_buf2, shmid_buf3, shmid_buf4;
    
    printf("consumer %d launched. total consumers=%d\n", me+1, c_total);
    
    //access shared memory for turn variable (int)
    int shmid_turn = shmget(SHMKEY_TURN, BUFF_SZ, 0777);
        if (shmid_turn == -1) { //terminate if shmget failed
            perror("Error in consumer shmget turn");
            return 1;
        }
    int *turn = (int*) shmat(shmid_turn, 0, 0);
    
    //access to shared memory for flag array
    int shmid_flagarr = shmget(SHMKEY_FLAGARR, 20, 0777);
    if (shmid_flagarr == -1) { //terminate if shmget failed
        perror("Error in shmget");
        return 1;
    }
    int* flag;
    flag = shmat(shmid_flagarr, 0, 0);
    
    //access to buffer flag array
    int shmid_bufflags = shmget(SHMKEY_BUFFLAGS, 5*BUFF_SZ, 0777);
    if (shmid_bufflags == -1) { //terminate if shmget failed
        perror("Error in shmget for bufflags");
        return 1;
    }
    int* buf_flags;
    buf_flags = shmat(shmid_bufflags, 0, 0);
    
    //get shared memory locations for buffers
    shmid_buf0 = shmget(SHMKEY_buf0, 100, 0777);
    if (shmid_buf0 == -1) { //terminate if shmget failed
            perror("Error in shmget for buf0");
            return 1;
        }
    char* buf0;
    buf0 = shmat(shmid_buf0, 0, 0);
    shmid_buf1 = shmget(SHMKEY_buf1, 100, 0777);
    if (shmid_buf1 == -1) { //terminate if shmget failed
        perror("Error in shmget for buf1");
        return 1;
    }
    char* buf1;
    buf1 = shmat(shmid_buf1, 0, 0);
    shmid_buf2 = shmget(SHMKEY_buf2, 100, 0777);
    if (shmid_buf2 == -1) { //terminate if shmget failed
        perror("Error in shmget for buf2");
        return 1;
    }
    char* buf2;
    buf2 = shmat(shmid_buf2, 0, 0);
    shmid_buf3 = shmget(SHMKEY_buf3, 100, 0777);
    if (shmid_buf3 == -1) { //terminate if shmget failed
         error("Error in shmget for buf3");
         return 1;
    }
    char* buf3;
    buf3 = shmat(shmid_buf3, 0, 0);
    shmid_buf4 = shmget(SHMKEY_buf4, 100, 0777);
    if (shmid_buf4 == -1) { //terminate if shmget failed
        perror("Error in shmget for buf4");
        return 1;
    }
    char* buf4;
    buf4 = shmat(shmid_buf4, 0, 0);
    
    //multiprocess solution to mutual exclusion taken from lecture notes
    //me is this consumer's number
    enum state {idle, want_in, in_cs};
    int j; //it's this consumer's turn
    
    do {
        do {
            flag[me] = want_in; //raise my flag
            j = *turn; //set local variable
            //now wait until it's my turn
            while (j != me) {
                j = (flag[j] != idle) ? *turn : (j + 1) % c_total;
            }
            
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
        if (buf_flags[0] == 1) {
            printf("consumer %d: emptying buffer 0 of %s", me+1, buf0);
            buf_flags[0] = 0;
        }
        else if (buf_flags[1] == 1) {
            printf("consumer %d: emptying buffer 1 of %s", me+1, buf1);
            buf_flags[1] = 0;
        }
        else if (buf_flags[2] == 1) {
            printf("consumer %d: emptying buffer 2 of %s", me+1, buf2);
            buf_flags[2] = 0;
        }
        else if (buf_flags[3] == 1) {
            printf("consumer %d: emptying buffer 3 of %s", me+1, buf3);
            buf_flags[3] = 0;
        }
        else if (buf_flags[4] == 1) {
            printf("consumer %d: emptying buffer 4 of %s", me+1, buf4);
            buf_flags[4] = 0;
        }
        //else printf("Consumer %d: All buffers are empty. ", me+1);
        
        //exit section
        j = (*turn + 1) % c_total;
        while (flag[j] == idle)
            j = (j + 1) % c_total;
        
        //assign turn to next waiting process & change my flag to idle
        *turn = j;
        flag[me] = idle;
        
        //remainder section: sleepy time
        srand((me+1)*13);
        randomTime = rand() %5 + 1;
        printf("Consumer %d: sleeping %d sec...\n",me+1, randomTime);
        sleep(randomTime);
    } while (1);
    
    printf("consumer %d terminated: finished. pid: %ld\n", me+1, getpid());
    return 1;
}

