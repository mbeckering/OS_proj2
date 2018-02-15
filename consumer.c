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

#define SHMKEY 859047
#define SHMKEY2 859050
#define BUFF_SZ sizeof (int)

/*
 * 
 */
int main(int argc, char** argv) {
    int c_num = atoi(argv[1]);
    int proc_limit = atoi(argv[2]);
    int i;
    printf("consumer %d: sleeping\n", c_num);
    sleep(2);
    
    //access shared memory for turn variable
    int shmid_turn = shmget(SHMKEY, BUFF_SZ, 0777);
        if (shmid_turn == -1) { //terminate if shmget failed
            perror("Error in consumer shmget turn");
            return 1;
        }
    
    //access to shared memory for flag array
    int shmid_flagarr = shmget(SHMKEY2, BUFF_SZ, 0777);
    printf("shmid_flagarr = %d\n", shmid_turn);
    if (shmid_flagarr == -1) { //terminate if shmget failed
        perror("Error in shmget");
        return 1;
    }
    //get pointer to the shared flag array 
    int* flag;
    flag = shmat(shmid_flagarr, 0, 0);
    
    //flag array access testing
    printf("consumer %d: flag array access testing:\n", c_num);
    for (i=0; i<proc_limit; i++) {
            printf("flag[%d]=%d, ", i, flag[i]);
        }
    printf("\n");
    
    //get pointer to shared int "turn"
    char * paddr = (char*)(shmat (shmid_turn, 0, 0) );
    int * turn = (int*) (paddr);
    *turn = c_num + 420;
    
    //printf("consumer %d launched. pid=%ld shmid_turn=%d turn=%d\n", c_num, getpid(), shmid_turn, (int)*turn);
    
    printf("consumer %d terminated: normal. pid: %ld\n", c_num, getpid());
    return 1;
}

