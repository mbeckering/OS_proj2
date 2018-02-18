/* 
 * File:   producer.c
 * Author: Michael Beckering
 *
 * Created on February 13, 2018, 10:09 AM
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
    printf("producer launched: pid: %ld\n", getpid());
    int randomTime, i;
    int shmid_buf0, shmid_buf1, shmid_buf2, shmid_buf3, shmid_buf4;
    char strin[100];
    FILE *fp;
    
    //set file pointer to strings.data
    fp = fopen("strings.data", "r");
    if (fp == NULL) {
        perror("producer: error opening file");
        return -1;
    }
    
    //set up shared memory for bufflags
    
    int shmid_bufflags = shmget(SHMKEY_BUFFLAGS, 5*BUFF_SZ, 0777);
    if (shmid_bufflags == -1) { //terminate if shmget failed
        perror("Error in shmget for bufflags");
        return 1;
    }
    //char * paddr = (char*) shmat(shmid_bufflags, 0, 0);
    //int* buf_flags = (int*)(paddr);
    int* buf_flags;
    buf_flags = shmat(shmid_bufflags, 0, 0);

    //initialize buffer flags to 0 (empty)
    /*
    for (i=0; i<5; i++) {
        buf_flags[i] = 0;
        printf("producer: bufflags[%d] = %d\n", i, buf_flags[i]);
    }
    */
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
    
    /*
    if ( fgets(strin, 99, fp) == NULL) {
        printf("producer: Error: strings.data is empty\n");
        return 1;
    }
    printf("strin set to: %s\n", strin);
     * */
    
    int doloop = 0;
    int forloop = 0;
    //enum state {empty, full};
    
    while (fgets(strin, 99, fp) != NULL) { //if a buffer is free, fill it
        printf("doloop %d, strin=%s\n", doloop, strin);
            if (buf_flags[0] == 0) {//if buffer 0 is open
                printf("inbuf0\n");
                buf_flags[0] = 1;
                sprintf(buf0, "%s", strin);
                printf("buf0 is now: %s. fgetting...", buf0);
                if (fgets(strin, 99, fp) == NULL)
                    break;
                printf("buf0 is now: %s. strin is now: %s", buf0, strin);
            }
            if ( (buf_flags[1] == 0)) {
                printf("inbuf1\n");
                buf_flags[1] = 1;
                sprintf(buf1, "%s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
                printf("buf1 is now: %s. strin is now: %s", buf1, strin);
            }
            if ( (buf_flags[2] == 0)) {
                printf("inbuf2\n");
                buf_flags[2] = 1;
                sprintf(buf2, "%s", strin);;
                if (fgets(strin, 99, fp) == NULL)
                    break;
                printf("buf2 is now: %s. strin is now: %s", buf2, strin);
            }
            if ( (buf_flags[3] == 0)) {
                printf("inbuf3\n");
                buf_flags[3] = 1;
                sprintf(buf3, "%s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
                printf("strin is now: %s", strin);
            }
            if ( (buf_flags[4] == 0) ) {
                printf("inbuf4\n");
                buf_flags[4] = 1;
                sprintf(buf4, "%s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
                printf("strin is now: %s", strin);
            }
        doloop++;
    }
    /*
    buf0 = "test0";
    buf1 = "test1";
    buf2 = "test2";
    buf3 = "test3";
    buf4 = "test4";
    */
    printf("buf_flags[0] = %d, buf0 = %s", buf_flags[0], buf0);
    printf("buf_flags[1] = %d, buf1 = %s", buf_flags[1], buf1);
    printf("buf_flags[2] = %d, buf2 = %s", buf_flags[2], buf2);
    printf("buf_flags[3] = %d, buf3 = %s", buf_flags[3], buf3);
    printf("buf_flags[4] = %d, buf4 = %s", buf_flags[4], buf4);
        
    printf("producer: EoF reached, terminating. pid: %ld\n", getpid());

    return (EXIT_SUCCESS);
}
/*
for (i=0; i<5; i++) { //if a buffer is free, fill it
            
            if (buf_flags[i] == 0) {//if buffer 0 is open
                
            }
            /*
            else if ( (buf_flags[i] == 0) && (i == 1) ) {
                buf_flags[i] = 1;
                buf1 = strin;
            }
            else if ( (buf_flags[i] == 0) && (i == 2) ) {
                buf_flags[i] = 1;
                buf2 = strin;
            }
            else if ( (buf_flags[i] == 0) && (i == 3) ) {
                buf_flags[i] = 1;
                buf3 = strin;
            }
            else if ( (buf_flags[i] == 0) && (i == 4) ) {
                buf_flags[i] = 1;
                buf4 = strin;
            }
            
            else printf("producer: no empty buffers\n");
             * */
/*
do { //read & loop until EoF
        if (buf_flags[0] == 0) {
            buf0 = strin;
            buf_flags[0] = 1;
            printf("buf4=%s", buf4);
            if ( fgets(strin, 99, fp) == NULL)
                break;
        }
        if (buf_flags[1] == 0) {
            buf1 = strin;
            buf_flags[1] = 1;
            if ( fgets(strin, 99, fp) == NULL)
                break;
        }
        if (buf_flags[2] == 0) {
            buf2 = strin;
            buf_flags[2] = 1;
            if ( fgets(strin, 99, fp) == NULL)
                break;
        }
        if (buf_flags[3] == 0) {
            buf3 = strin;
            buf_flags[3] = 1;
            if ( fgets(strin, 99, fp) == NULL)
                break;
        }
        if (buf_flags[4] == 0) {
            buf4 = strin;
            buf_flags[4] = 1;
            if ( fgets(strin, 99, fp) == NULL)
                break;
        }
            randomTime = rand() %2 + 1;
            printf("Producer sleeping %d sec...\n", randomTime);
            sleep(randomTime);
        } while (fgets(strin, 99, fp) != NULL);
 */