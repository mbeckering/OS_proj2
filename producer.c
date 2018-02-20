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
#include <time.h>
#include <signal.h>

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
void siginthandler(int sig_num);
char *getTime(void);
static FILE *fp; //pointer to input file
static FILE *plog; //pointer to log file

int main(int argc, char** argv) {
    int randomTime, i;
    int shmid_buf0, shmid_buf1, shmid_buf2, shmid_buf3, shmid_buf4;
    char strin[100];
    
    //set up sigint handler
    signal (SIGINT, siginthandler);
    
    //set file pointer to strings.data
    fp = fopen("strings.data", "r");
    if (fp == NULL) {
        perror("producer: error opening data file");
        return -1;
    }
    //create log file
    plog = fopen("prod.log", "w");
    if (plog == NULL) {
        perror("producer: error opening log file");
        return -1;
    }
    //write launch to log file
    fprintf(plog, "%s ", getTime());
    fprintf(plog, "Started\n");
    
    //access to shared memory for flag array
    int shmid_flagarr = shmget(SHMKEY_FLAGARR, 24, 0777);
    if (shmid_flagarr == -1) { //terminate if shmget failed
        perror("Error in shmget");
        return 1;
    }
    int* flag;
    flag = shmat(shmid_flagarr, 0, 0);
    
    //set up shared memory for bufflags
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
    
    int doloop = 0;
    int forloop = 0;
    int allfull = 0;
    //enum state {empty, full};
    
    if (fgets(strin, 99, fp) == NULL) {
        printf("Producer: Error: strings.data is empty \n");
    }
    
    while (1) {
        while(1){
            allfull = 1;
            fprintf(plog, "%s ", getTime());
            fprintf(plog, "Check\n");
            for (i=0; i<5; i++) {
                if (buf_flags[i] == 0) {
                    allfull = 0;
                }
            }
            if (allfull == 1) {
                 randomTime = rand() %3 + 1;
                 fprintf(plog, "%s ", getTime());
                 fprintf(plog, "Sleep %d\n", randomTime);
                 sleep(randomTime);
            }
            else if (allfull == 0) {
                break;
            }
        }
        
            if (buf_flags[0] == 0) {//if buffer 0 is open
                sprintf(buf0, "%s", strin);
                buf_flags[0] = 1;
                fprintf(plog, "%s ", getTime());
                fprintf(plog, "Write 0 %s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
            }
            if (buf_flags[1] == 0) {
                sprintf(buf1, "%s", strin);
                buf_flags[1] = 1;
                fprintf(plog, "%s ", getTime());
                fprintf(plog, "Write 1 %s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
            }
            if (buf_flags[2] == 0) {
                sprintf(buf2, "%s", strin);;
                buf_flags[2] = 1;
                fprintf(plog, "%s ", getTime());
                fprintf(plog, "Write 2 %s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
            }
            if (buf_flags[3] == 0) {
                sprintf(buf3, "%s", strin);
                buf_flags[3] = 1;
                fprintf(plog, "%s ", getTime());
                fprintf(plog, "Write 3 %s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
            }
            if (buf_flags[4] == 0) {
                sprintf(buf4, "%s", strin);
                buf_flags[4] = 1;
                fprintf(plog, "%s ", getTime());
                fprintf(plog, "Write 4 %s", strin);
                if (fgets(strin, 99, fp) == NULL)
                    break;
            }
        doloop++;
    }
    printf("producer: End of file reached...\n");
    fprintf(plog, "%s ", getTime());
    fprintf(plog, "Sleep 6\n");
    sleep(6);//sleep long enough to ensure consumers clear all 5 buffers
    flag[5] = 9;//signal to consumers that EoF has been reached
    fprintf(plog, "%s ", getTime());
    fprintf(plog, "Terminated Normal\n");
    fclose(plog);
    fclose(fp);

    return (EXIT_SUCCESS);
}

//this function pulled from stackoverflow but edited for output format
char *getTime(void) {
    
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    static char _retval[20];
    strftime(_retval, sizeof(_retval), "%H:%M:%S", timeinfo);
    
    return _retval;
}

//SIGINT handler
void siginthandler(int sig_num) {
    int sh_status, i;
    pid_t sh_wpid;
    
    //print to log, close file and shut down
    fprintf(plog, "%s ", getTime());
    fprintf(plog, "Terminated Interrupted\n");
    fclose(plog);
    fclose(fp);
    exit(0);
}