/* 
 * File:   producer.c
 * Author: Michael Beckering
 *
 * Created on February 13, 2018, 10:09 AM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {
    
    printf("producer launched: pid: %ld\n", getpid());
    printf("producer shutting down. pid: %ld\n", getpid());

    return (EXIT_SUCCESS);
}

